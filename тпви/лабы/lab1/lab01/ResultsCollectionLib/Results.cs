using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace ResultsCollectionLib
{
    public class Results
    {
        private string _jsonPath;
        private List<Result> _results = new List<Result>();
        private int _maxKey = 1;
        private readonly object _lock = new();

        public Results(string jsonPath)
        {
            _jsonPath = jsonPath;
            if (File.Exists(jsonPath))
            {
                var jsontext = File.ReadAllText(jsonPath);
                _results = JsonConvert.DeserializeObject<List<Result>>(jsontext) ?? new List<Result>();
                _maxKey = _results.Count == 0 ? 1 : _results.Max(r => r.Id) + 1;
            }
            else
            {
                _results = new List<Result>();
                _maxKey = 1;
            }
        }

        public List<Result> GetAllResults()
        {
            return _results.Select(r => new Result { Id = r.Id, Value = r.Value }).ToList();
        }
        public Result? GetResultById(int id)
        {
            return _results.FirstOrDefault(r => r.Id == id);
        }
        public void AddResult(string value)
        {
            lock (_lock)
            {
                _results.Add(new Result { Id = _maxKey++, Value = value });
                IsSerialized();
            }
        }

        public void ChangeResultById(int id, string value)
        {
            lock (_lock)
            {
                var element = GetResultById(id);
                if (element == null)
                {
                    Console.WriteLine("There is no such an element");
                }
                else
                {
                    element.Value = value;
                    IsSerialized();
                }
            }
        }

        public void DeleteResult(int id)
        {
            lock (_lock)
            {
                var result = GetResultById(id);
                if (result == null)
                {
                    Console.WriteLine("There is no such an element");
                }
                else
                {
                    _results.Remove(result);
                    IsSerialized();
                }
            }
        }

        private void IsSerialized()
        {
            bool success = Serialize();
            if (!success)
                Console.WriteLine("Not done");
            else
                Console.WriteLine("Done");
        }

        private bool Serialize()
        {
            try
            {
                var json = JsonConvert.SerializeObject(_results, Formatting.Indented);
                File.WriteAllText(_jsonPath, json);
                return true;
            }
            catch
            {
                return false;
            }
        }

    }
}
