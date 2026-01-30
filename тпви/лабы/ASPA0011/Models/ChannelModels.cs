namespace ASPA0011.Models
{
    // JSON-1
    public class ChannelModel
    {
        public Guid Id { get; set; }
        public string Name { get; set; } = "";
        public string Description { get; set; } = "";
        public string State { get; set; } = "";
    }

    // JSON-2
    public class CreateChannelRequest
    {
        public string Name { get; set; } = "";
        public string State { get; set; } = "";
        public string Description { get; set; } = "";
    }

    // JSON-3,4,5,6 - Stop/Resume requests
    public class ChannelStateRequest
    {
        public string Action { get; set; } = ""; // "stop", "resume"
        public string Reason { get; set; } = "";
        public Guid? ChannelId { get; set; }
    }

    // JSON-7,8 - Delete requests  
    public class DeleteChannelsRequest
    {
        public string Type { get; set; } = ""; // "all", "closed"
    }

    // JSON-9 - Dequeue/Peek request
    public class QueueOperationRequest
    {
        public string Operation { get; set; } = ""; // "dequeue", "peek"
        public Guid ChannelId { get; set; }
    }

    // JSON-10 - Enqueue request
    public class EnqueueRequest
    {
        public Guid ChannelId { get; set; }
        public string Data { get; set; } = "";
    }

    // JSON-11 - Queue response
    public class QueueResponse
    {
        public string? Data { get; set; }
        public string Status { get; set; } = "";
        public bool Success { get; set; }
    }

    // Service results
    public class ChannelsResult
    {
        public List<ChannelModel>? Channels { get; set; }
        public string Status { get; set; } = "";
    }

    public class ChannelResult
    {
        public ChannelModel? Channel { get; set; }
        public string Status { get; set; } = "";
    }
}