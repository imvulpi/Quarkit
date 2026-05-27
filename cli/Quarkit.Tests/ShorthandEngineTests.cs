using Quarkit.Core.Shorthand;

namespace Quarkit.Tests;

public class ShorthandEngineTests
{
    public const string SOFTWARE_NAME = "Quarkit";
    public const string VERSION = "1.0.0";
    public const string TARGET_CPU = "x64";
    public const string SPACES4 = "    ";

    private ShorthandEngine CreateEngine()
    {
        var engine = new ShorthandEngine();
        engine.SetToken("<SoftwareName>", SOFTWARE_NAME);
        engine.SetToken("<Version>", VERSION);
        engine.SetToken("<TargetCpu>", TARGET_CPU);
        engine.SetToken("<Spaces4>", SPACES4);
        return engine;
    }

    [Test]
    public async Task SettingTokens_ShouldAddTokensCorrectly()
    {
        var engine = new ShorthandEngine();
        engine.SetToken("<SoftwareName>", SOFTWARE_NAME);
        engine.SetToken("<Version>", $"{VERSION}");
        engine.SetToken("<TargetCpu>", $"{TARGET_CPU}");
        await Assert.That(engine.tokensAndValues.Count).IsEqualTo(3);
    }

    [Test]
    [Arguments("Normal text without any tags.", "Normal text without any tags.")]
    [Arguments("<SoftwareName>", SOFTWARE_NAME)] // Tag at the absolute start and end
    [Arguments("<SoftwareName> v<Version>", $"{SOFTWARE_NAME} v{VERSION}")] // Multiple tags in same text
    [Arguments("<SoftwareName><Version><TargetCpu>", $"{SOFTWARE_NAME}{VERSION}{TARGET_CPU}")] // Multiple tags directly adjacent
    [Arguments("<Spaces4>", SPACES4)]
    public async Task StandardParsing_ShouldResolveCorrectly(string input, string expected)
    {
        var engine = CreateEngine();
        var result = engine.Expand(input);
        await Assert.That(result).IsEqualTo(expected);
    }

    [Test]
    [Arguments("A < B and path is <SoftwareName>", $"A < B and path is {SOFTWARE_NAME}")] // Standalone < followed by valid tag
    [Arguments("Nested <<SoftwareName>", $"Nested <{SOFTWARE_NAME}")] // Double open-bracket reset logic
    [Arguments("Unclosed tag at the end <SoftwareName", "Unclosed tag at the end <SoftwareName")] // Missing closing bracket
    [Arguments("<SoftwareName unclosed start", "<SoftwareName unclosed start")] // Only opening bracket at start
    public async Task NestedAndMalformedBrackets_ShouldResolveIntuitively(string input, string expected)
    {
        var engine = CreateEngine();
        var result = engine.Expand(input);
        await Assert.That(result).IsEqualTo(expected);
    }

    [Test]
    [Arguments("This is a <invalid tag name> with spaces", "This is a <invalid tag name> with spaces")] // Spaces inside tags ignored
    [Arguments("< Soft> and <Soft >", "< Soft> and <Soft >")] // Leading/trailing spaces inside brackets treated as plain text
    [Arguments("<UnregisteredTag>", "<UnregisteredTag>")] // Safe fallback if token doesn't exist in engine map
    public async Task InvalidTagsAndSpaces_ShouldBeTreatedAsPlainStrings(string input, string expected)
    {
        var engine = CreateEngine();
        var result = engine.Expand(input);
        await Assert.That(result).IsEqualTo(expected);
    }

    [Test]
    [Arguments(@"\<SoftwareName\>", "<SoftwareName>")] // Escaped tags treated as regular string
    [Arguments(@"Text with missing escape closing \<SoftwareName>", "Text with missing escape closing <SoftwareName>")]
    [Arguments(@"\<SoftwareName\> at start and end \<SoftwareName\>", "<SoftwareName> at start and end <SoftwareName>")] // Escaped tags boundaries
    public async Task EscapedTags_ShouldRenderAsLiteralBrackets(string input, string expected)
    {
        var engine = CreateEngine();
        var result = engine.Expand(input);
        await Assert.That(result).IsEqualTo(expected);
    }

    [Test]
    [Arguments(@"\\<SoftwareName>", @$"\{SOFTWARE_NAME}")] // Escaping an escape makes the tag valid again
    [Arguments(@"\\\\<SoftwareName>", @$"\\{SOFTWARE_NAME}")] // Four backslashes yield two literal backslashes + expanded tag
    [Arguments(@"\\", @"\")] // Just an escaped backslash
    [Arguments(@"\\\<SoftwareName\>", @"\<SoftwareName>")] // Two backslashes (literal '\') + one backslash escaping the tag
    public async Task BackslashEscapes_ShouldResolveToCorrectLiteralFractions(string input, string expected)
    {
        var engine = CreateEngine();
        var result = engine.Expand(input);
        await Assert.That(result).IsEqualTo(expected);
    }

    [Test]
    [Arguments("Mix: \\<Escaped\\> and <SoftwareName> with \\\\<Version>", @$"Mix: <Escaped> and {SOFTWARE_NAME} with \{VERSION}")] // Regular string (not RAW @)
    public async Task ChaosComplexString_ShouldParseFlawlesslyInOnePass(string input, string expected)
    {
        var engine = CreateEngine();
        var result = engine.Expand(input);
        await Assert.That(result).IsEqualTo(expected);
    }
}