using System.Threading.Channels;
using Microsoft.Extensions.Logging;

namespace ASPA0011.Models
{
    public class ChannelService
    {
        private readonly object _lock = new object();
        private readonly ILogger<ChannelService> _logger;
        private readonly int _waitEnqueue;
        private int _eventCounter = 0;
        
        private List<ChannelModel> _channels = new();
        private Dictionary<Guid, Channel<string>> _channelInstances = new();

        public ChannelService(IConfiguration config, ILogger<ChannelService> logger)
        {
            _logger = logger;
            _waitEnqueue = config.GetValue("WaitEnqueue", 30);
            _logger.LogTrace("[{Counter}] Service started", ++_eventCounter);
        }

        public List<ChannelModel> GetAllChannels()
        {
            lock (_lock)
            {
                return _channels.ToList();
            }
        }

        public ChannelModel? GetChannelById(Guid id)
        {
            lock (_lock)
            {
                return _channels.FirstOrDefault(x => x.Id == id);
            }
        }

        public ChannelResult CreateChannel(CreateChannelRequest request)
        {
            _logger.LogDebug("[{Counter}] CreateChannel", ++_eventCounter);

            lock (_lock)
            {
                var id = Guid.NewGuid();
                var channel = Channel.CreateUnbounded<string>();
                
                var newChannel = new ChannelModel
                {
                    Id = id,
                    Name = request.Name,
                    Description = request.Description,
                    State = request.State?.ToUpper() ?? "ACTIVE"
                };
                
                _channels.Add(newChannel);
                _channelInstances[id] = channel;

                _logger.LogInformation("[{Counter}] Channel created: {Id}", ++_eventCounter, id);

                return new ChannelResult { Channel = newChannel, Status = "created" };
            }
        }

        public ChannelsResult UpdateChannelsState(ChannelStateRequest request)
        {
            _logger.LogDebug("[{Counter}] UpdateChannelsState: {Action}", ++_eventCounter, request.Action);

            lock (_lock)
            {
                if (request.ChannelId.HasValue)
                {
                    var channel = _channels.FirstOrDefault(x => x.Id == request.ChannelId.Value);
                    if (channel == null)
                        return new ChannelsResult { Channels = null, Status = "channel not found" };

                    if (request.Action == "stop" && channel.State == "ACTIVE")
                    {
                        if (_channelInstances.ContainsKey(channel.Id))
                        {
                            _channelInstances[channel.Id].Writer.Complete();
                        }
                        channel.State = "CLOSED";
                    }
                    else if (request.Action == "resume" && channel.State == "CLOSED")
                    {
                        _channelInstances[channel.Id] = Channel.CreateUnbounded<string>();
                        channel.State = "ACTIVE";
                    }

                    return new ChannelsResult { Channels = new List<ChannelModel> { channel }, Status = "success" };
                }
                else
                {
                    foreach (var channel in _channels)
                    {
                        if (request.Action == "stop" && channel.State == "ACTIVE")
                        {
                            if (_channelInstances.ContainsKey(channel.Id))
                            {
                                _channelInstances[channel.Id].Writer.Complete();
                            }
                            channel.State = "CLOSED";
                        }
                        else if (request.Action == "resume" && channel.State == "CLOSED")
                        {
                            _channelInstances[channel.Id] = Channel.CreateUnbounded<string>();
                            channel.State = "ACTIVE";
                        }
                    }
                    return new ChannelsResult { Channels = _channels, Status = "success" };
                }
            }
        }

        public ChannelsResult DeleteChannels(DeleteChannelsRequest request)
        {
            _logger.LogDebug("[{Counter}] DeleteChannels: {Type}", ++_eventCounter, request.Type);

            lock (_lock)
            {
                var deletedChannels = new List<ChannelModel>();

                if (request.Type == "all")
                {
                    deletedChannels = _channels.ToList();
                    foreach (var channel in _channelInstances.Values)
                    {
                        try { channel.Writer.Complete(); } catch { }
                    }
                    _channelInstances.Clear();
                    _channels.Clear();
                }
                else if (request.Type == "closed")
                {
                    for (int i = _channels.Count - 1; i >= 0; i--)
                    {
                        if (_channels[i].State == "CLOSED")
                        {
                            deletedChannels.Add(_channels[i]);
                            var channelId = _channels[i].Id;
                            if (_channelInstances.ContainsKey(channelId))
                            {
                                try { _channelInstances[channelId].Writer.Complete(); } catch { }
                                _channelInstances.Remove(channelId);
                            }
                            _channels.RemoveAt(i);
                        }
                    }
                }

                return new ChannelsResult { Channels = deletedChannels, Status = "success" };
            }
        }

        public QueueResponse ProcessQueueOperation(QueueOperationRequest request)
        {
            _logger.LogDebug("[{Counter}] ProcessQueueOperation: {Operation}", ++_eventCounter, request.Operation);

            lock (_lock)
            {
                var channel = _channels.FirstOrDefault(x => x.Id == request.ChannelId);
                if (channel == null)
                    return new QueueResponse { Success = false, Status = "channel not found" };

                if (!_channelInstances.ContainsKey(request.ChannelId))
                    return new QueueResponse { Success = false, Status = "channel instance not found" };

                var channelInstance = _channelInstances[request.ChannelId];
                
                if (request.Operation == "dequeue")
                {
                    return channelInstance.Reader.TryRead(out var data) 
                        ? new QueueResponse { Data = data, Success = true, Status = "success" }
                        : new QueueResponse { Success = false, Status = "no data available" };
                }
                else if (request.Operation == "peek")
                {
                    return channelInstance.Reader.TryPeek(out var data) 
                        ? new QueueResponse { Data = data, Success = true, Status = "success" }
                        : new QueueResponse { Success = false, Status = "no data available" };
                }

                return new QueueResponse { Success = false, Status = "invalid operation" };
            }
        }

        public async Task<QueueResponse> EnqueueMessage(EnqueueRequest request)
        {
            _logger.LogDebug("[{Counter}] EnqueueMessage for channel: {ChannelId}", ++_eventCounter, request.ChannelId);

            Channel<string>? channelInstance;
            lock (_lock)
            {
                var channel = _channels.FirstOrDefault(x => x.Id == request.ChannelId);
                if (channel == null)
                {
                    _logger.LogWarning("[{Counter}] Channel not found: {ChannelId}", ++_eventCounter, request.ChannelId);
                    return new QueueResponse { Success = false, Status = "channel not found" };
                }

                if (!_channelInstances.ContainsKey(request.ChannelId))
                {
                    _logger.LogWarning("[{Counter}] Channel instance not found: {ChannelId}", ++_eventCounter, request.ChannelId);
                    return new QueueResponse { Success = false, Status = "channel instance not found" };
                }

                channelInstance = _channelInstances[request.ChannelId];
            }

            var cts = new CancellationTokenSource(TimeSpan.FromSeconds(_waitEnqueue));

            try
            {
                if (await channelInstance.Writer.WaitToWriteAsync(cts.Token))
                {
                    if (channelInstance.Writer.TryWrite(request.Data))
                    {
                        _logger.LogInformation("[{Counter}] Message enqueued: {ChannelId}", ++_eventCounter, request.ChannelId);
                        return new QueueResponse { Success = true, Status = "enqueued" };
                    }
                }
            }
            catch (OperationCanceledException)
            {
                _logger.LogWarning("[{Counter}] Enqueue timeout: {ChannelId}", ++_eventCounter, request.ChannelId);
                return new QueueResponse { Success = false, Status = "timeout" };
            }

            _logger.LogWarning("[{Counter}] Enqueue failed: {ChannelId}", ++_eventCounter, request.ChannelId);
            return new QueueResponse { Success = false, Status = "write failed" };
        }
    }
}