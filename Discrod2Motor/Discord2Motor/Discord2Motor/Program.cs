using Discord;
using Discord.Net;
using Discord.WebSocket;
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

    //Only needed once to register our command once
    private async Task SetupGlobalApplicationCommands()
    {
        List<ApplicationCommandProperties> applicationCommandProperties = new();

        var moveCommand = new SlashCommandBuilder();
        moveCommand.WithName("move");
        moveCommand.WithDescription("Handles Motor Movements!");
        moveCommand.AddOption("x", ApplicationCommandOptionType.Integer, "X Position to move Motor", isRequired: false, minValue: 0, maxValue: 100);
        moveCommand.AddOption("y", ApplicationCommandOptionType.Integer, "Y Rotation to rotate Camera", isRequired: false, minValue: -90, maxValue: 90);

        applicationCommandProperties.Add(moveCommand.Build());

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
        Console.WriteLine($"Sending message over comport: {message}");
        _serialPort.Write(message + END_MARKER);
    }
}