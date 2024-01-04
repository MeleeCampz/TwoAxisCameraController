using Newtonsoft.Json;

namespace Discord2Motor
{
    [Serializable]
    public struct StoredCommand
    {
        public string Name;
        public int X;
        public int Y;

        public bool IsValid;

        public StoredCommand(string name, int x, int y)
        {
            Name = name.ToLowerInvariant();
            X = x;
            Y = y;
            IsValid = true;
        }

        public override string ToString()
        {
            return $"NAME: {Name} | X: {X}| Y: {Y}";
        }
    }

    [Serializable]
    public struct StoredCommands
    {
        public List<StoredCommand> Commands;
    }

    internal class FileIO
    {
        public const string ConfigFolder = "Save";
        public const string ConfigFile = ConfigFolder + "/StoredCommandes.json";

        private static bool _isInitiated = false;
        private static readonly Dictionary<string, StoredCommand> _currentCommands = new();

        private static async Task InitializeDictionaryFromFile()
        {
            if (_isInitiated) return;

            try
            {
                var dicInfo = new DirectoryInfo(ConfigFolder);
                if(!dicInfo.Exists)
                {
                    dicInfo.Create();
                }

                string content = await File.ReadAllTextAsync(ConfigFile);
                var commands = JsonConvert.DeserializeObject<StoredCommands>(content);

                _currentCommands.Clear();
                foreach (var one in commands.Commands)
                {
                    _currentCommands.Add(one.Name, one);
                }
                _isInitiated = true;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        public static async Task<List<StoredCommand>> GetStoredCommands()
        {
            await InitializeDictionaryFromFile();

            return _currentCommands.Values.ToList();
        }

        public static async Task AddOrOverrideCommand(StoredCommand command)
        {
            if (!command.IsValid) return;

            await InitializeDictionaryFromFile();

            _currentCommands[command.Name] = command;

            var storedCommands = new StoredCommands();
            storedCommands.Commands = _currentCommands.Values.ToList();

            try
            {
                string asText = JsonConvert.SerializeObject(storedCommands);
                File.WriteAllText(ConfigFile, asText);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        public static async Task<StoredCommand> LoadStoredCommand(string name)
        {
            name = name.ToLowerInvariant();

            await InitializeDictionaryFromFile();

            if (_currentCommands.TryGetValue(name, out var storedCommand))
            {
                return storedCommand;
            }

            return new();
        }
    }
}
