namespace ASPA0011_1.Models
{
    public class AspChannel
    {
        public Guid Id { get; set; }
        public string Name { get; set; } = "";
        public string Desc { get; set; } = "";
        public string State { get; set; } = "";
    }

    public class CreateModel
    {
        public string Cmd { get; set; } = "";
        public string Name { get; set; } = "";
        public string State { get; set; } = "";
        public string Desc { get; set; } = "";
    }

    public class StopAllModel
    { 
        public string Cmd { get; set; } = "";
        public string Reason { get; set; } = "";
    }

    public class StopOneModel
    {
        public string Cmd { get; set; } = "";
        public Guid Id { get; set; }
        public string Reason { get; set; } = "";
    }

    public class ResumeAllModel
    {
        public string Cmd { get; set; } = "";
    }

    public class ResumeOneModel
    {
        public string Cmd { get; set; } = "";
        public Guid Id { get; set; }
        public string State { get; set; } = "";
    }

    public class DeleteAllModel
    {
        public string Cmd { get; set; } = "";
    }

    public class DeleteClosedModel
    {
        public string Cmd { get; set; } = "";
        public string State { get; set; } = "";
    }

    public class QueueCmdModel
    {
        public string Cmd { get; set; } = "";  
        public Guid Id { get; set; }         
    }

    public class QueueEnqueueModel
    {
        public string Cmd { get; set; } = "";  
        public Guid Id { get; set; }          
        public string Data { get; set; } = "";      
    }

    // Results
    public class CreateResult
    {
        public AspChannel? Channel { get; set; }
        public string Status { get; set; } = "";
    }

    public class StopAllResult
    {
        public List<AspChannel>? Channels { get; set; }
        public string Status { get; set; } = "";
        public string? Reason { get; set; }
    }

    public class StopOneResult
    {
        public AspChannel? Channel { get; set; }
        public string Status { get; set; } = "";
    }

    public class ResumeAllResult
    {
        public List<AspChannel>? Channels { get; set; }
        public string Status { get; set; } = "";
    }

    public class ResumeOneResult
    {
        public AspChannel? Channel { get; set; }
        public string Status { get; set; } = "";
    }

    public class DequeuePeekResult
    {
        public string? Data { get; set; }
        public string Status { get; set; } = "";
    }

    public class EnqueueResult
    {
        public bool Success { get; set; }
        public string Status { get; set; } = "";
    }

    public class QueueOperationRequest
    {
        public string Operation { get; set; } = ""; // "dequeue", "peek"
        public Guid ChannelId { get; set; }
    }
    
    public class EnqueueRequest
    {
        public Guid ChannelId { get; set; }
        public string Data { get; set; } = "";
    }
    
    public class QueueResponse
    {
        public string? Data { get; set; }
        public string Status { get; set; } = "";
        public bool Success { get; set; }
    }
}