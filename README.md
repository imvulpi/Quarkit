# Quarkit (In Development)

**The cross-platform installer generator for modern desktop applications.**  
Pack your native C, C++, C#, Rust, or Go apps into lightweight installers and packages!

---

### Status

**Quarkit is currently in pre-alpha development.** 
I'm actively designing the core C source, options and planning things out.

* ⭐ **Star this project!** to track development progress and get notified when it's ready!
* 💬 **Have suggestions/ideas?** [Join discord](https://discord.gg/6YcByUjpdR) and discuss it with me.

---

## Why Quarkit?

The idea is to create very lightweight installers for desktop applications - so that they're not bigger than the apps themselves 😄 The existing tools either bundle heavy runtimes, force you to learn a scripting language or lack multi-platform support. 

Quarkit is created to offer a modern, sub-100KB, cross-platform alternative.

### How Quarkit Compares

| Feature / Metric | Quarkit | NSIS / Inno Setup | Velopack / Electron Builder |
| :--- | :--- | :--- | :--- |
| **Added Installer Size** | **< 100 KB** | **~50KB** | **~10-50+ MB** |
| **Cross-Platform** | **Yes** (Win, macOS, Linux) | Windows | Partial / Heavy configuration |
| **Setup Complexity** | **Single `app.json` File** | Complex custom script languages | Heavy build pipelines & dependencies |
| **Theme & UI Modernization**| **Modern & Customizable** | Dated, legacy UI out of the box | Customizable, but extremely heavy |
| **Price / License** | **100% Free & Open Source (MPL 2.0)** | Free (Inno heavily suggest buying a license) | Usually Free |

---

## Planned Features:

* **Sub-100KB:** Pure, native C installer stub for Windows with zero runtime dependencies.
* **Linux & macOS Packaging:** Auto-generate `.deb`, `.dmg`, `.AppImage`, and `.pkg` files directly from your CLI.
* **Clean, Customizable UI:** Easily styleble Windows wizard with modern dark/light themes and custom branding with no scripting required.
* **Single Config Setup:** One human-readable `app.json` builds target installers across all platforms.

---

## Proposed Usage (CLI Preview)

```bash
# Build installers and platform packages for all targets
quarkit build --config app.json

# Build specifically for Windows
quarkit build --target windows
```

## Talks and Reasons

I build a desktop app that's a local server for my project: **Penvia**, of course I wanted to make an installer so people don't have to manually go and extract the files to their disks. I didn't like NSIS although the size is super small (50KB is impressive), because the UI looks outdated and it only works on Windows while I wanted to expand to Linux. I had a similiar story with Inno Setup but with the quirk that they very heavily suggest to pay for the license, which I thought was a bad sign for an opensource project. I understand supporting projects like Inno Setup but I believe that it's a bit too heavy - almost like some ad, while in reality it's a donation since it's free/opensource.

I decided on **Velopack** and it was great... until I noticed that my small 3MB binary turned into a whopping 10MB one, a very cool feature that their team developed was **delta** updates, a delta of my app was JUST 1.7MB. Wow that really sounds like an advertise for velopack now, so what was the issue you ask? You see if I were to just compress the 3MB app with LZMA2 then it turns into a 1.3MB file, so even delta is bigger than my actual app when compressed. I do believe that Velopack is still great if you own a big app, the delta feature might be useful to you. The final issue with Velopack is lack of customization, their direction seems to be going in towards a kind of a silent installer, but what if I wanted to display EULA, ToS etc? No luck there. Images can be displayed but their position, loading bar etc are very limited. I might be wrong but I think with newer versions of velopack the binary output size increased.

Alright now a 10MB app is already somewhat big - doesn't compare to electron apps though. You really have to appreciate how developers used to have apps smaller than that with full fledged and great UI. With Quarkit I want to allow developers to create stub, gui and silent installers to add more to it, the installer can launch an app with an argument that states that it's freshly after install. What does that add you ask? You can make your own UI after install and only after install to reduce the size of the installer even more. My goal is for the installer itself to be under 50KB, although useful options could make it be more. I already tested how much an algorithm for Tar, Zip, Lzma adds and it's definitely possible - I also wrote [my own small algorithm for tar](https://github.com/imvulpi/MiniPaxTar), feel free to check it out to see my C code.

I would write Quarkit in C while using neccesary compression, tar and other libraries. I did once attempt to optimize xz lzma2 which did remove a couple of KB but with the big disadvantage of a maintaince hell. Installer is what would be the most optimized, written with a system in mind, meaning nostdlib. The cli tool would be made with stdlib since the size of that doesn't matter AS MUCH.

So what do you think?
[Tell me in discord here](https://discord.gg/6YcByUjpdR).