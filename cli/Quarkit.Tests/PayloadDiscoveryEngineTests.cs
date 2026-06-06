using Quarkit.Core.Discovery;
using Quarkit.Models.Core.Target;
using Quarkit.Tests.Mocks;

namespace Quarkit.Tests;

public class PayloadDiscoveryEngineTests
{
    [Test]
    public async Task DiscoverPayloads_ShouldScanAndResolveValidTargetOutputs()
    {
        var mockFs = new MockFileSystem();
        var discoveryEngine = new PayloadDiscoveryEngine(mockFs);

        string searchRoot = "C:/Project/Release";
        string executableSuffix = "publish/myapp.exe";

        mockFs.WriteAllText("C:/Project/Release/win-x64/publish/myapp.exe", "fake-exe-binary-data");
        mockFs.WriteAllText("C:/Project/Release/linux-arm/publish/myapp.exe", "fake-elf-binary-data");
        mockFs.CreateDirectory("C:/Project/Release/logs-cache");

        List<DiscoveredPayload> payloadsFound = discoveryEngine.DiscoverPayloads(searchRoot, executableSuffix);

        await Assert.That(payloadsFound.Count).EqualTo(2);

        var winPayload = payloadsFound.Find(p => p.FolderName == "win-x64");
        await Assert.That(winPayload).IsNotNull();
        await Assert.That(winPayload!.Target.System).IsEqualTo(OSSystem.Windows);
        await Assert.That(winPayload.Target.Bit).IsEqualTo(Bitness.x64);
        await Assert.That(MockFileSystem.Normalize(winPayload.AbsolutePayloadPath)).IsEqualTo("C:/Project/Release/win-x64/publish/myapp.exe");

        var linuxPayload = payloadsFound.Find(p => p.FolderName == "linux-arm");
        await Assert.That(linuxPayload).IsNotNull();
        await Assert.That(linuxPayload!.Target.System).IsEqualTo(OSSystem.Linux);
        await Assert.That(linuxPayload.Target.Arch).IsEqualTo(Architecture.Arm);
    }
}
