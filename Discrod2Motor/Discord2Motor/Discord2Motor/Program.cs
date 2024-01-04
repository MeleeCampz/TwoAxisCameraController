using Discord;
using Discord.Net;
using Discord.WebSocket;
using Discord2Motor;
using Newtonsoft.Json;
using System.IO.Ports;

public class Program
{
    public static Task Main(string[] args) => new Program().MainAsync();

    private DiscordSocketClient? _client;
    private SerialPort? _serialPort;

    public async Task MainAsync()
    {
        var socketContent = new DiscordSocketConfig();
        socketContent.GatewayIntents = GatewayIntents.MessageContent | GatewayIntents.AllUnprivileged;

        string comPort = File.ReadAllText("Config/MotorConfig.config");
        Console.WriteLine($"Opening Serial connection to: {comPort}");

        try
        {
            _serialPort = new SerialPort(comPort);
            _serialPort.BaudRate = 115200;
            _serialPort.Open();
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }

        _client = new DiscordSocketClient(socketContent);
        _client.Log += Log;

        // Use this to set up commands once
        //_client.Ready += SetupGlobalApplicationCommands;

        var token = File.ReadAllText("Config/BotToken.config");
        await _client.LoginAsync(TokenType.Bot, token);
        await _client.StartAsync();


        _client.SlashCommandExecuted += Client_SlashCommandExecuted;

        // Block this task until the program is closed.
        await Task.Delay(-1);
    }

    private async Task Client_SlashCommandExecuted(SocketSlashCommand command)
    {
        switch (command.CommandName)
        {
            case "move":
                await ExecuteMoveCommand(command);
                break;
            case "store":
                await ExecuteStoreCommand(command);
                break;
            case "load":
                await ExecuteLoadCommand(command);
                break;
            case "list":
                await ExecuteListCommand(command);
                break;
            default:
                break;
        }
    }

    private async Task ExecuteMoveCommand(SocketSlashCommand command)
    {
        string x = "-";
        string y = "-";

        foreach (var one in command.Data.Options)
        {
            if (one.Name == "x")
            {
                long target = (long)one.Value;
                x = target.ToString();
                SendMotorXCommand(target);
            }
            else if (one.Name == "y")
            {
                long target = (long)one.Value;
                y = target.ToString();
                SendMotorYCommand(target);
            }
        }

        await command.RespondAsync($"Moving: X:{x} Y:{y}");
    }
    private async Task ExecuteStoreCommand(SocketSlashCommand command)
    {
        string? name = null;
        int? x = null;
        int? y = null;

        foreach (var one in command.Data.Options)
        {
            if (one.Name == "x")
            {
                x = (int)(long)one.Value;
            }
            else if (one.Name == "y")
            {
                y = (int)(long)one.Value;
            }
            else if(one.Name == "name")
            {
                name = (string)one.Value;
            }
        }

        if(name != null && x.HasValue && y.HasValue)
        {
            StoredCommand sc = new StoredCommand(name, x.Value, y.Value);
            await FileIO.AddOrOverrideCommand(sc);
            await command.RespondAsync($"Stored Command:  {sc}");
        }
        else
        {
            await command.RespondAsync($"Invalid argumetns to safe command!");
        }
    }

    private async Task ExecuteLoadCommand(SocketSlashCommand command)
    {
        string? name = null;

        foreach (var one in command.Data.Options)
        {
            if (one.Name == "name")
            {
                name = (string)one.Value;
                break;
            }
        }

        if (name != null)
        {
            var storedCommand = await FileIO.LoadStoredCommand(name);
            if (storedCommand.IsValid)
            {
                SendMotorXCommand(storedCommand.X);
                SendMotorYCommand(storedCommand.Y);
                await command.RespondAsync($"Moving: X:{storedCommand.X} Y:{storedCommand.Y}");
            }
            else
            {
                await command.RespondAsync($"Failed to laod command with name: {name}");
            }
        }
    }

    private static async Task ExecuteListCommand(SocketSlashCommand command)
    {
        var options = await FileIO.GetStoredCommands();

        string display = "Commands: \r\n";

        foreach(var one in options)
        {
            display += one.ToString() + "\r\n";
        }

        await command.RespondAsync(display);
    }

    //Only needed once to register our command once
    private async Task SetupGlobalApplicationCommands()
    {
        const string X_DESC = "X Position to move Motor";
        const string Y_DESC = "Y Rotation to rotate Camera";
        const string STORE_DESC = "Name to be used to load stored parameters later again";
        const string LOAD_DESC = "Name that was previously used to store coordinates";

        const int MIN_X = 0;
        const int MAX_X = 100;

        const int MIN_Y = -180;
        const int MAX_Y = 180;

        List<ApplicationCommandProperties> applicationCommandProperties = new();

        var moveCommand = new SlashCommandBuilder();
        moveCommand.WithName("move");
        moveCommand.WithDescription("Handles Motor Movements!");
        moveCommand.AddOption("x", ApplicationCommandOptionType.Integer, X_DESC, isRequired: false, minValue: MIN_X, maxValue: MAX_X);
        moveCommand.AddOption("y", ApplicationCommandOptionType.Integer, Y_DESC, isRequired: false, minValue: MIN_Y, maxValue: MAX_Y);

        var storeCommand = new SlashCommandBuilder();
        storeCommand.WithName("store");
        storeCommand.WithDescription("Assosiates or Overrides givven X and Y Coordinates with Name");
        storeCommand.AddOption("x", ApplicationCommandOptionType.Integer, X_DESC, isRequired: true, minValue: MIN_X, maxValue: MAX_X);
        storeCommand.AddOption("y", ApplicationCommandOptionType.Integer, Y_DESC, isRequired: true, minValue: MIN_Y, maxValue: MAX_Y);
        storeCommand.AddOption("name", ApplicationCommandOptionType.String, STORE_DESC, isRequired: true, minLength: 3);

        var loadCommand = new SlashCommandBuilder();
        loadCommand.WithName("load");
        loadCommand.WithDescription("Loads Coordinates that where previouly created using the store command");
        loadCommand.AddOption("name", ApplicationCommandOptionType.String, LOAD_DESC, isRequired: true, minLength: 3);

        var listCommand = new SlashCommandBuilder();
        listCommand.WithName("list");
        listCommand.WithDescription("Lists all coordinates that were created using the store command");

        applicationCommandProperties.Add(moveCommand.Build());
        applicationCommandProperties.Add(storeCommand.Build());
        applicationCommandProperties.Add(loadCommand.Build());
        applicationCommandProperties.Add(listCommand.Build());

        try
        {
            await _client.BulkOverwriteGlobalApplicationCommandsAsync(applicationCommandProperties.ToArray());
        }
        catch (HttpException exception)
        {
            var json = JsonConvert.SerializeObject(exception.Errors, Formatting.Indented);
            Console.WriteLine(json);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }
    }

    private Task Log(LogMessage msg)
    {
        Console.WriteLine(msg.ToString());
        return Task.CompletedTask;
    }

    private void SendMotorXCommand(long value) => SendMotorCommand('x', value);

    private void SendMotorYCommand(long value) => SendMotorCommand('y', value);

    const char END_MARKER = '\n';
    private void SendMotorCommand(char motor, long value)
    {
        string message = motor + value.ToString();

        if(_serialPort == null || !_serialPort.IsOpen)
        {
            Console.WriteLine($"Serial port invalid or not open! Cannot send command: {message}");
            return;
        }

        Console.WriteLine($"Sending message over comport: {message}");
        _serialPort.Write(message + END_MARKER);
    }
}