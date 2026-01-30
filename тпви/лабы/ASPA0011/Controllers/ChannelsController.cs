using Microsoft.AspNetCore.Mvc;
using ASPA0011.Models;

namespace ASPA0011.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ChannelsController : ControllerBase
    {
        private readonly ChannelService _service;
        private readonly ILogger<ChannelsController> _logger;
        private int _counter = 0;

        public ChannelsController(ChannelService service, ILogger<ChannelsController> logger)
        {
            _service = service;
            _logger = logger;
        }

        [HttpGet]
        [ProducesResponseType(200)]
        [ProducesResponseType(204)]
        public ActionResult<List<ChannelModel>> GetAll()
        {
            _logger.LogTrace("[{Counter}] {Time} - GetAll", ++_counter, DateTime.Now);
            
            var channels = _service.GetAllChannels();
            return channels.Count > 0 ? Ok(channels) : NoContent();
        }
        
        [HttpGet("{id}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public ActionResult<ChannelModel> GetById(Guid id)
        {
            _logger.LogTrace("[{Counter}] {Time} - GetById {Id}", ++_counter, DateTime.Now, id);
            
            var channel = _service.GetChannelById(id);
            if (channel == null)
            {
                _logger.LogError("[{Counter}] {Time} - Channel not found: {Id}", 
                    ++_counter, DateTime.Now, id);
                return NotFound();
            }
            return Ok(channel);
        }

        // POST /api/channels
        [HttpPost]
        [ProducesResponseType(201)]
        [ProducesResponseType(204)]
        public ActionResult<ChannelModel> Create([FromBody] CreateChannelRequest request)
        {
            _logger.LogTrace("[{Counter}] Create", ++_counter);

            var result = _service.CreateChannel(request);

            if (result.Channel == null)
                return BadRequest();

            return result.Channel.State == "ACTIVE"
                ? CreatedAtAction(nameof(GetById), new { id = result.Channel.Id }, result.Channel)
                : NoContent();
        }

        // PUT /api/channels
        [HttpPut]
        [ProducesResponseType(200)]
        public ActionResult<ChannelsResult> UpdateState([FromBody] ChannelStateRequest request)
        {
            _logger.LogTrace("[{Counter}] UpdateState", ++_counter);

            var result = _service.UpdateChannelsState(request);
            return Ok(result);
        }

        // DELETE /api/channels
        [HttpDelete]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public ActionResult<ChannelsResult> Delete([FromBody] DeleteChannelsRequest request)
        {
            _logger.LogTrace("[{Counter}] Delete", ++_counter);

            var result = _service.DeleteChannels(request);
            return result.Channels?.Count > 0 ? Ok(result) : NotFound();
        }
    }
}