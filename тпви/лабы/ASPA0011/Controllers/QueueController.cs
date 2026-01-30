using Microsoft.AspNetCore.Mvc;
using ASPA0011.Models;

namespace ASPA0011.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class QueueController : ControllerBase
    {
        private readonly ChannelService _service;
        private readonly ILogger<QueueController> _logger;
        private int _counter = 0;

        public QueueController(ChannelService service, ILogger<QueueController> logger)
        {
            _service = service;
            _logger = logger;
        }

        [HttpPost]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public ActionResult<QueueResponse> ProcessQueueOperation([FromBody] QueueOperationRequest request)
        {
            _logger.LogTrace("[{Counter}] {Time} - ProcessQueueOperation {Operation}", 
                ++_counter, DateTime.Now, request.Operation);

            var result = _service.ProcessQueueOperation(request);
            return result.Success ? Ok(result) : NotFound(result);
        }

        [HttpPost("enqueue")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public async Task<IActionResult> Enqueue([FromBody] EnqueueRequest request)
        {
            _logger.LogTrace("[{Counter}] {Time} - Enqueue {ChannelId}", 
                ++_counter, DateTime.Now, request.ChannelId);

            var result = await _service.EnqueueMessage(request);

            if (result.Success)
            {
                _logger.LogTrace("[{Counter}] {Time} - Enqueue success", 
                    ++_counter, DateTime.Now);
                return Ok(result);
            }
            else
            {
                _logger.LogError("[{Counter}] {Time} - Enqueue failed: {Status}", 
                    ++_counter, DateTime.Now, result.Status);
                return NotFound(result);
            }
        }
    }
}