/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2017 Baldur Karlsson
 * Copyright (c) 2014 Crytek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#include <fstream>
#include <sstream>
#include "api/replay/renderdoc_replay.h"
#include "api/replay/version.h"
#include "common/common.h"
#include "core/core.h"
#include "maths/camera.h"
#include "maths/formatpacking.h"
#include "replay/type_helpers.h"
#include "serialise/string_utils.h"

// these entry points are for the replay/analysis side - not for the application.

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC Topology_NumVerticesPerPrimitive(Topology topology)
{
  // strips/loops/fans have the same number of indices for a single primitive
  // as their list friends
  switch(topology)
  {
    default:
    case Topology::Unknown: break;
    case Topology::PointList: return 1;
    case Topology::LineList:
    case Topology::LineStrip:
    case Topology::LineLoop: return 2;
    case Topology::TriangleList:
    case Topology::TriangleStrip:
    case Topology::TriangleFan: return 3;
    case Topology::LineList_Adj:
    case Topology::LineStrip_Adj: return 4;
    case Topology::TriangleList_Adj:
    case Topology::TriangleStrip_Adj: return 6;
    case Topology::PatchList_1CPs:
    case Topology::PatchList_2CPs:
    case Topology::PatchList_3CPs:
    case Topology::PatchList_4CPs:
    case Topology::PatchList_5CPs:
    case Topology::PatchList_6CPs:
    case Topology::PatchList_7CPs:
    case Topology::PatchList_8CPs:
    case Topology::PatchList_9CPs:
    case Topology::PatchList_10CPs:
    case Topology::PatchList_11CPs:
    case Topology::PatchList_12CPs:
    case Topology::PatchList_13CPs:
    case Topology::PatchList_14CPs:
    case Topology::PatchList_15CPs:
    case Topology::PatchList_16CPs:
    case Topology::PatchList_17CPs:
    case Topology::PatchList_18CPs:
    case Topology::PatchList_19CPs:
    case Topology::PatchList_20CPs:
    case Topology::PatchList_21CPs:
    case Topology::PatchList_22CPs:
    case Topology::PatchList_23CPs:
    case Topology::PatchList_24CPs:
    case Topology::PatchList_25CPs:
    case Topology::PatchList_26CPs:
    case Topology::PatchList_27CPs:
    case Topology::PatchList_28CPs:
    case Topology::PatchList_29CPs:
    case Topology::PatchList_30CPs:
    case Topology::PatchList_31CPs:
    case Topology::PatchList_32CPs: return PatchList_Count(topology);
  }

  return 0;
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC Topology_VertexOffset(Topology topology,
                                                                     uint32_t primitive)
{
  // strips/loops/fans have the same number of indices for a single primitive
  // as their list friends
  switch(topology)
  {
    default:
    case Topology::Unknown:
    case Topology::PointList:
    case Topology::LineList:
    case Topology::TriangleList:
    case Topology::LineList_Adj:
    case Topology::TriangleList_Adj:
    case Topology::PatchList_1CPs:
    case Topology::PatchList_2CPs:
    case Topology::PatchList_3CPs:
    case Topology::PatchList_4CPs:
    case Topology::PatchList_5CPs:
    case Topology::PatchList_6CPs:
    case Topology::PatchList_7CPs:
    case Topology::PatchList_8CPs:
    case Topology::PatchList_9CPs:
    case Topology::PatchList_10CPs:
    case Topology::PatchList_11CPs:
    case Topology::PatchList_12CPs:
    case Topology::PatchList_13CPs:
    case Topology::PatchList_14CPs:
    case Topology::PatchList_15CPs:
    case Topology::PatchList_16CPs:
    case Topology::PatchList_17CPs:
    case Topology::PatchList_18CPs:
    case Topology::PatchList_19CPs:
    case Topology::PatchList_20CPs:
    case Topology::PatchList_21CPs:
    case Topology::PatchList_22CPs:
    case Topology::PatchList_23CPs:
    case Topology::PatchList_24CPs:
    case Topology::PatchList_25CPs:
    case Topology::PatchList_26CPs:
    case Topology::PatchList_27CPs:
    case Topology::PatchList_28CPs:
    case Topology::PatchList_29CPs:
    case Topology::PatchList_30CPs:
    case Topology::PatchList_31CPs:
    case Topology::PatchList_32CPs:
      // for all lists, it's just primitive * Topology_NumVerticesPerPrimitive(topology)
      break;
    case Topology::LineStrip:
    case Topology::LineLoop:
    case Topology::TriangleStrip:
    case Topology::LineStrip_Adj:
      // for strips, each new vertex creates a new primitive
      return primitive;
    case Topology::TriangleStrip_Adj:
      // triangle strip with adjacency is a special case as every other
      // vert is purely for adjacency so it's doubled
      return primitive * 2;
    case Topology::TriangleFan: RDCERR("Cannot get VertexOffset for triangle fan!"); break;
  }

  return primitive * Topology_NumVerticesPerPrimitive(topology);
}

extern "C" RENDERDOC_API float RENDERDOC_CC Maths_HalfToFloat(uint16_t half)
{
  return ConvertFromHalf(half);
}

extern "C" RENDERDOC_API uint16_t RENDERDOC_CC Maths_FloatToHalf(float f)
{
  return ConvertToHalf(f);
}

extern "C" RENDERDOC_API Camera *RENDERDOC_CC Camera_InitArcball()
{
  return new Camera(Camera::eType_Arcball);
}

extern "C" RENDERDOC_API Camera *RENDERDOC_CC Camera_InitFPSLook()
{
  return new Camera(Camera::eType_FPSLook);
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_Shutdown(Camera *c)
{
  delete c;
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_SetPosition(Camera *c, float x, float y, float z)
{
  c->SetPosition(Vec3f(x, y, z));
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_SetFPSRotation(Camera *c, float x, float y, float z)
{
  c->SetFPSRotation(Vec3f(x, y, z));
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_SetArcballDistance(Camera *c, float dist)
{
  c->SetArcballDistance(dist);
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_ResetArcball(Camera *c)
{
  c->ResetArcball();
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_RotateArcball(Camera *c, float ax, float ay,
                                                                float bx, float by)
{
  c->RotateArcball(Vec2f(ax, ay), Vec2f(bx, by));
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_GetBasis(Camera *c, FloatVector *pos,
                                                           FloatVector *fwd, FloatVector *right,
                                                           FloatVector *up)
{
  Vec3f p = c->GetPosition();
  Vec3f f = c->GetForward();
  Vec3f r = c->GetRight();
  Vec3f u = c->GetUp();

  pos->x = p.x;
  pos->y = p.y;
  pos->z = p.z;

  fwd->x = f.x;
  fwd->y = f.y;
  fwd->z = f.z;

  right->x = r.x;
  right->y = r.y;
  right->z = r.z;

  up->x = u.x;
  up->y = u.y;
  up->z = u.z;
}

extern "C" RENDERDOC_API const char *RENDERDOC_CC RENDERDOC_GetVersionString()
{
  return MAJOR_MINOR_VERSION_STRING;
}

extern "C" RENDERDOC_API const char *RENDERDOC_CC RENDERDOC_GetConfigSetting(const char *name)
{
  return RenderDoc::Inst().GetConfigSetting(name).c_str();
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_SetConfigSetting(const char *name,
                                                                      const char *value)
{
  RenderDoc::Inst().SetConfigSetting(name, value);
}

extern "C" RENDERDOC_API void *RENDERDOC_CC RENDERDOC_MakeEnvironmentModificationList(int numElems)
{
  rdctype::array<EnvironmentModification> *ret = new rdctype::array<EnvironmentModification>();
  create_array_uninit(*ret, (size_t)numElems);
  return ret;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_SetEnvironmentModification(
    void *mem, int idx, const char *variable, const char *value, EnvMod type, EnvSep separator)
{
  rdctype::array<EnvironmentModification> *mods = (rdctype::array<EnvironmentModification> *)mem;

  mods->elems[idx] = EnvironmentModification(type, separator, variable, value);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_FreeEnvironmentModificationList(void *mem)
{
  rdctype::array<EnvironmentModification> *mods = (rdctype::array<EnvironmentModification> *)mem;
  delete mods;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_SetDebugLogFile(const char *log)
{
  if(log)
    RDCLOGFILE(log);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_LogText(const char *text)
{
  rdclog_int(LogType::Comment, "EXT", "external", 0, "%s", text);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_LogMessage(LogType type, const char *project,
                                                                const char *file, unsigned int line,
                                                                const char *text)
{
  rdclog_int(type, project ? project : "UNK?", file ? file : "unknown", line, "%s", text);

#if ENABLED(DEBUGBREAK_ON_ERROR_LOG)
  if(type == LogType::Error)
    RDCBREAK();
#endif

  if(type == LogType::Fatal)
    RDCDUMP();
}

extern "C" RENDERDOC_API const char *RENDERDOC_CC RENDERDOC_GetLogFile()
{
  return RDCGETLOGFILE();
}

extern "C" RENDERDOC_API void RENDERDOC_CC
RENDERDOC_InitGlobalEnv(GlobalEnvironment env, const rdctype::array<rdctype::str> &args)
{
  std::vector<std::string> argsVec;
  argsVec.reserve(args.size());
  for(const rdctype::str &a : args)
    argsVec.push_back(a.c_str());

  RenderDoc::Inst().ProcessGlobalEnvironment(env, argsVec);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_TriggerExceptionHandler(void *exceptionPtrs,
                                                                             bool32 crashed)
{
  if(RenderDoc::Inst().GetCrashHandler() == NULL)
    return;

  if(exceptionPtrs)
  {
    RenderDoc::Inst().GetCrashHandler()->WriteMinidump(exceptionPtrs);
  }
  else
  {
    if(!crashed)
    {
      RDCLOG("Writing crash log");
    }

    RenderDoc::Inst().GetCrashHandler()->WriteMinidump();

    if(!crashed)
    {
      RenderDoc::Inst().RecreateCrashHandler();
    }
  }
}

extern "C" RENDERDOC_API ReplaySupport RENDERDOC_CC RENDERDOC_SupportLocalReplay(
    const char *logfile, rdctype::str *driver, rdctype::str *recordMachineIdent)
{
  ICaptureFile *file = RENDERDOC_OpenCaptureFile(logfile);

  if(driver)
    *driver = file->DriverName();

  if(recordMachineIdent)
    *recordMachineIdent = file->RecordedMachineIdent();

  ReplaySupport support = file->LocalReplaySupport();

  file->Shutdown();

  return support;
}

extern "C" RENDERDOC_API ReplayStatus RENDERDOC_CC
RENDERDOC_CreateReplayRenderer(const char *logfile, float *progress, IReplayController **rend)
{
  ICaptureFile *file = RENDERDOC_OpenCaptureFile(logfile);

  ReplayStatus ret = file->OpenStatus();

  if(ret != ReplayStatus::Succeeded)
  {
    file->Shutdown();
    return ret;
  }

  std::tie(ret, *rend) = file->OpenCapture(progress);

  file->Shutdown();

  return ret;
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC
RENDERDOC_ExecuteAndInject(const char *app, const char *workingDir, const char *cmdLine,
                           const rdctype::array<EnvironmentModification> &env, const char *logfile,
                           const CaptureOptions &opts, bool32 waitForExit)
{
  return Process::LaunchAndInjectIntoProcess(app, workingDir, cmdLine, env, logfile, opts,
                                             waitForExit != 0);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_GetDefaultCaptureOptions(CaptureOptions *opts)
{
  *opts = CaptureOptions();
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_StartGlobalHook(const char *pathmatch,
                                                                     const char *logfile,
                                                                     const CaptureOptions &opts)
{
  Process::StartGlobalHook(pathmatch, logfile, opts);
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC
RENDERDOC_InjectIntoProcess(uint32_t pid, const rdctype::array<EnvironmentModification> &env,
                            const char *logfile, const CaptureOptions &opts, bool32 waitForExit)
{
  return Process::InjectIntoProcess(pid, env, logfile, opts, waitForExit != 0);
}

extern "C" RENDERDOC_API bool32 RENDERDOC_CC RENDERDOC_GetThumbnail(const char *filename,
                                                                    FileType type, uint32_t maxsize,
                                                                    rdctype::array<byte> *buf)
{
  ICaptureFile *file = RENDERDOC_OpenCaptureFile(filename);

  if(file->OpenStatus() != ReplayStatus::Succeeded)
  {
    file->Shutdown();
    return false;
  }

  *buf = file->GetThumbnail(type, maxsize);
  file->Shutdown();
  return true;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_FreeArrayMem(const void *mem)
{
  rdctype::array<char>::deallocate(mem);
}

extern "C" RENDERDOC_API void *RENDERDOC_CC RENDERDOC_AllocArrayMem(uint64_t sz)
{
  return rdctype::array<char>::allocate((size_t)sz);
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC RENDERDOC_EnumerateRemoteTargets(const char *host,
                                                                                uint32_t nextIdent)
{
  string s = "localhost";
  if(host != NULL && host[0] != '\0')
    s = host;

  // initial case is we're called with 0, start with the first port.
  // otherwise we're called with the last successful ident, so increment
  // before continuing to enumerate.
  if(nextIdent == 0)
    nextIdent = RenderDoc_FirstTargetControlPort;
  else
    nextIdent++;

  uint32_t lastIdent = RenderDoc_LastTargetControlPort;
  if(host != NULL && Android::IsHostADB(host))
  {
    int index = 0;
    std::string deviceID;
    Android::extractDeviceIDAndIndex(host, index, deviceID);

    // each subsequent device gets a new range of ports. The deviceID isn't needed since we already
    // forwarded the ports to the right devices.
    if(nextIdent == RenderDoc_FirstTargetControlPort)
      nextIdent += RenderDoc_AndroidPortOffset * (index + 1);
    lastIdent += RenderDoc_AndroidPortOffset * (index + 1);

    s = "127.0.0.1";
  }

  for(; nextIdent <= lastIdent; nextIdent++)
  {
    Network::Socket *sock = Network::CreateClientSocket(s.c_str(), (uint16_t)nextIdent, 250);

    if(sock)
    {
      SAFE_DELETE(sock);
      return nextIdent;
    }
  }

  // tried all idents remaining and found nothing
  return 0;
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC RENDERDOC_GetDefaultRemoteServerPort()
{
  return RenderDoc_RemoteServerPort;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_BecomeRemoteServer(const char *listenhost,
                                                                        uint32_t port,
                                                                        volatile bool32 *killReplay)
{
  bool32 dummy = false;

  if(killReplay == NULL)
    killReplay = &dummy;

  if(listenhost == NULL || listenhost[0] == 0)
    listenhost = "0.0.0.0";

  if(port == 0)
    port = RENDERDOC_GetDefaultRemoteServerPort();

  RenderDoc::Inst().BecomeRemoteServer(listenhost, (uint16_t)port, *killReplay);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_StartSelfHostCapture(const char *dllname)
{
  void *module = Process::LoadModule(dllname);

  if(module == NULL)
    return;

  pRENDERDOC_GetAPI get =
      (pRENDERDOC_GetAPI)Process::GetFunctionAddress(module, "RENDERDOC_GetAPI");

  if(get == NULL)
    return;

  RENDERDOC_API_1_0_0 *rdoc = NULL;

  get(eRENDERDOC_API_Version_1_0_0, (void **)&rdoc);

  if(rdoc == NULL)
    return;

  rdoc->StartFrameCapture(NULL, NULL);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_EndSelfHostCapture(const char *dllname)
{
  void *module = Process::LoadModule(dllname);

  if(module == NULL)
    return;

  pRENDERDOC_GetAPI get =
      (pRENDERDOC_GetAPI)Process::GetFunctionAddress(module, "RENDERDOC_GetAPI");

  if(get == NULL)
    return;

  RENDERDOC_API_1_0_0 *rdoc = NULL;

  get(eRENDERDOC_API_Version_1_0_0, (void **)&rdoc);

  if(rdoc == NULL)
    return;

  rdoc->EndFrameCapture(NULL, NULL);
}

namespace Android
{
bool IsHostADB(const char *hostname)
{
  return !strncmp(hostname, "adb:", 4);
}
void extractDeviceIDAndIndex(const string &hostname, int &index, string &deviceID)
{
  if(!IsHostADB(hostname.c_str()))
    return;

  const char *c = hostname.c_str();
  c += 4;

  index = atoi(c);

  c = strchr(c, ':');

  if(!c)
  {
    index = 0;
    return;
  }

  c++;

  deviceID = c;
}
Process::ProcessResult execCommand(const string &cmd, const string &workDir = ".")
{
  RDCLOG("COMMAND: %s", cmd.c_str());

  size_t firstSpace = cmd.find(" ");
  string exe = cmd.substr(0, firstSpace);
  string args = cmd.substr(firstSpace + 1, cmd.length());

  Process::ProcessResult result;
  Process::LaunchProcess(exe.c_str(), workDir.c_str(), args.c_str(), &result);
  return result;
}
Process::ProcessResult adbExecCommand(const string &device, const string &args)
{
  string adbExePath = RenderDoc::Inst().GetConfigSetting("adbExePath");
  if(adbExePath.empty())
  {
    string exepath;
    FileIO::GetExecutableFilename(exepath);
    string exedir = dirname(FileIO::GetFullPathname(exepath));

    string adbpath = exedir + "/android/adb.exe";
    if(FileIO::exists(adbpath.c_str()))
      adbExePath = adbpath;

    if(adbExePath.empty())
    {
      static bool warnPath = true;
      if(warnPath)
      {
        RDCWARN("adbExePath not set, attempting to call 'adb' in working env");
        warnPath = false;
      }
      adbExePath = "adb";
    }
  }
  Process::ProcessResult result;
  string deviceArgs;
  if(device.empty())
    deviceArgs = args;
  else
    deviceArgs = StringFormat::Fmt("-s %s %s", device.c_str(), args.c_str());
  return execCommand(string(adbExePath + " " + deviceArgs).c_str());
}
string adbGetDeviceList()
{
  return adbExecCommand("", "devices").strStdout;
}
void adbForwardPorts(int index, const std::string &deviceID)
{
  int offs = RenderDoc_AndroidPortOffset * (index + 1);
  adbExecCommand(deviceID,
                 StringFormat::Fmt("forward tcp:%i tcp:%i", RenderDoc_RemoteServerPort + offs,
                                   RenderDoc_RemoteServerPort));
  adbExecCommand(deviceID,
                 StringFormat::Fmt("forward tcp:%i tcp:%i", RenderDoc_FirstTargetControlPort + offs,
                                   RenderDoc_FirstTargetControlPort));
}
uint32_t StartAndroidPackageForCapture(const char *host, const char *package)
{
  int index = 0;
  std::string deviceID;
  Android::extractDeviceIDAndIndex(host, index, deviceID);

  string packageName = basename(string(package));    // Remove leading '/' if any

  adbExecCommand(deviceID, "shell am force-stop " + packageName);
  adbForwardPorts(index, deviceID);
  adbExecCommand(deviceID, "shell setprop debug.vulkan.layers VK_LAYER_RENDERDOC_Capture");
  // Creating the capture file
  adbExecCommand(deviceID,
                 "shell pm grant " + packageName + " android.permission.WRITE_EXTERNAL_STORAGE");
  // Reading the capture thumbnail
  adbExecCommand(deviceID,
                 "shell pm grant " + packageName + " android.permission.READ_EXTERNAL_STORAGE");
  adbExecCommand(deviceID,
                 "shell monkey -p " + packageName + " -c android.intent.category.LAUNCHER 1");

  uint32_t ret = RenderDoc_FirstTargetControlPort + RenderDoc_AndroidPortOffset * (index + 1);
  uint32_t elapsed = 0,
           timeout = 1000 *
                     RDCMAX(5, atoi(RenderDoc::Inst().GetConfigSetting("MaxConnectTimeout").c_str()));
  while(elapsed < timeout)
  {
    // Check if the target app has started yet and we can connect to it.
    ITargetControl *control = RENDERDOC_CreateTargetControl(host, ret, "testConnection", false);
    if(control)
    {
      control->Shutdown();
      break;
    }

    Threading::Sleep(1000);
    elapsed += 1000;
  }

  // Let the app pickup the setprop before we turn it back off for replaying.
  adbExecCommand(deviceID, "shell setprop debug.vulkan.layers :");

  return ret;
}

bool SearchForAndroidLayer(const string &deviceID, const string &location, const string &layerName)
{
  RDCLOG("Checking for layers in: %s", location.c_str());
  string findLayer =
      adbExecCommand(deviceID, "shell find " + location + " -name " + layerName).strStdout;
  if(!findLayer.empty())
  {
    RDCLOG("Found RenderDoc layer in %s", location.c_str());
    return true;
  }
  return false;
}

bool RemoveAPKSignature(const string &apk)
{
  RDCLOG("Checking for existing signature");

  // Get the list of files in META-INF
  string fileList = execCommand("aapt list " + apk).strStdout;
  if(fileList.empty())
    return false;

  // Walk through the output.  If it starts with META-INF, remove it.
  uint32_t fileCount = 0;
  uint32_t matchCount = 0;
  std::istringstream contents(fileList);
  string line;
  string prefix("META-INF");
  while(std::getline(contents, line))
  {
    line = trim(line);
    fileCount++;
    if(line.compare(0, prefix.size(), prefix) == 0)
    {
      RDCDEBUG("Match found, removing  %s", line.c_str());
      execCommand("aapt remove " + apk + " " + line);
      matchCount++;
    }
  }
  RDCLOG("%d files searched, %d removed", fileCount, matchCount);

  // Ensure no hits on second pass through
  RDCDEBUG("Walk through file list again, ensure signature removed");
  fileList = execCommand("aapt list " + apk).strStdout;
  std::istringstream recheck(fileList);
  while(std::getline(recheck, line))
  {
    if(line.compare(0, prefix.size(), prefix) == 0)
    {
      RDCERR("Match found, that means removal failed! %s", line.c_str());
      return false;
    }
  }
  return true;
}

bool AddLayerToAPK(const string &apk, const string &layerPath, const string &layerName,
                   const string &abi, const string &tmpDir)
{
  RDCLOG("Adding RenderDoc layer");

  // Create the required directory layout
  string local("lib/" + abi + "/" + layerName);
  string global(tmpDir + local);
  FileIO::CreateParentDirectory(global);
  FileIO::Copy(layerPath.c_str(), global.c_str(), true);

  Process::ProcessResult result = execCommand("aapt add " + apk + " " + local, tmpDir);

  if(result.strStdout.empty())
  {
    RDCERR("Failed to add layer to APK. STDERR: %s", result.strStderror.c_str());
    return false;
  }

  return true;
}

bool RealignAPK(const string &apk, string &alignedAPK, const string &tmpDir)
{
  // Re-align the APK for performance
  RDCLOG("Realigning APK");
  string errOut = execCommand("zipalign -f 4 " + apk + " " + alignedAPK, tmpDir).strStderror;

  if(!errOut.empty())
    return false;

  // Wait until the aligned version exists to proceed
  uint32_t elapsed = 0;
  uint32_t timeout = 10000;    // 10 seconds
  while(elapsed < timeout)
  {
    std::ifstream infile(alignedAPK.c_str());
    if(infile.good())
    {
      RDCLOG("Aligned APK ready to go, continuing...");
      return true;
    }

    Threading::Sleep(1000);
    elapsed += 1000;
  }

  RDCERR("Timeout reached aligning APK");
  return false;
}

#if ENABLED(RDOC_WIN32)
static string debugKey("%USERPROFILE%\\.android\\debug.keystore");
#else
static string debugKey("~/.android/debug.keystore");
#endif

bool DebugSignAPK(const string &apk, const string &workDir)
{
  RDCLOG("Signing with debug key");

// clang-format off
#if ENABLED(RDOC_WIN32)
  execCommand(
      "cmd /C apksigner.bat sign "
      "--ks " + debugKey + " "
      "--ks-pass pass:android "
      "--key-pass pass:android "
      "--ks-key-alias androiddebugkey " +
      apk + "\\",
      workDir);
#else
  execCommand(
      "bash -lc \"apksigner sign "
      "--ks " + debugKey + " "
      "--ks-pass pass:android "
      "--key-pass pass:android "
      "--ks-key-alias androiddebugkey " +
      apk + "\"",
      workDir);
#endif
  // clang-format on

  // Check for signature
  string list = execCommand("aapt list " + apk).strStdout;

  // Walk through the output.  If it starts with META-INF, we're good
  std::istringstream contents(list);
  string line;
  string prefix("META-INF");
  while(std::getline(contents, line))
  {
    if(line.compare(0, prefix.size(), prefix) == 0)
    {
      RDCLOG("Signature found, continuing...");
      return true;
    }
  }

  RDCERR("re-sign of APK failed!");
  return false;
}

bool UninstallOriginalAPK(const string &deviceID, const string &packageName, const string &workDir)
{
  RDCLOG("Uninstalling previous version of application");

  execCommand("adb uninstall " + packageName, workDir);

  // Wait until uninstall completes
  string uninstallResult;
  uint32_t elapsed = 0;
  uint32_t timeout = 10000;    // 10 seconds
  while(elapsed < timeout)
  {
    uninstallResult = adbExecCommand(deviceID, "shell pm path " + packageName).strStdout;
    if(uninstallResult.empty())
    {
      RDCLOG("Package removed");
      return true;
    }

    Threading::Sleep(1000);
    elapsed += 1000;
  }

  RDCERR("Uninstallation of APK failed!");
  return false;
}

bool ReinstallPatchedAPK(const string &deviceID, const string &apk, const string &abi,
                         const string &packageName, const string &workDir)
{
  RDCLOG("Reinstalling APK");

  execCommand("adb install --abi " + abi + " " + apk, workDir);

  // Wait until re-install completes
  string reinstallResult;
  uint32_t elapsed = 0;
  uint32_t timeout = 10000;    // 10 seconds
  while(elapsed < timeout)
  {
    reinstallResult = adbExecCommand(deviceID, "shell pm path " + packageName).strStdout;
    if(!reinstallResult.empty())
    {
      RDCLOG("Patched APK reinstalled, continuing...");
      return true;
    }

    Threading::Sleep(1000);
    elapsed += 1000;
  }

  RDCERR("Reinstallation of APK failed!");
  return false;
}

bool CheckPatchingRequirements()
{
  // check for aapt, zipalign, apksigner, debug key
  vector<string> requirements;
  vector<string> missingTools;
  requirements.push_back("aapt");
  requirements.push_back("zipalign");
  requirements.push_back("apksigner");
  requirements.push_back("java");
#if ENABLED(RDOC_WIN32)
  for(uint32_t i = 0; i < requirements.size(); i++)
  {
    if(execCommand("cmd /C where " + requirements[i] + " 2>nul").strStdout.empty())
      missingTools.push_back(requirements[i]);
  }

  if(execCommand("cmd /C IF EXIST " + debugKey + " ECHO debug key foud").strStdout.empty())
    missingTools.push_back(debugKey);
#else
  for(uint32_t i = 0; i < requirements.size(); i++)
  {
    if(execCommand("which " + requirements[i]).strStdout.empty())
      missingTools.push_back(requirements[i]);
  }

  if(execCommand("bash -lc \"stat " + debugKey + "\"").strStdout.empty())
    missingTools.push_back(debugKey);
#endif

  if(missingTools.size() > 0)
  {
    for(uint32_t i = 0; i < missingTools.size(); i++)
      RDCERR("Missing %s", missingTools[i].c_str());
    return false;
  }

  return true;
}

bool PullAPK(const string &deviceID, const string &pkgPath, const string &apk)
{
  RDCLOG("Pulling APK to patch");

  adbExecCommand(deviceID, "pull " + pkgPath + " " + apk);

  // Wait until the apk lands
  uint32_t elapsed = 0;
  uint32_t timeout = 10000;    // 10 seconds
  while(elapsed < timeout)
  {
    std::ifstream infile(apk.c_str());
    if(infile.good())
    {
      RDCLOG("Original APK ready to go, continuing...");
      return true;
    }

    Threading::Sleep(1000);
    elapsed += 1000;
  }

  RDCERR("Failed to pull APK");
  return false;
}

bool CheckPermissions(const string &apk)
{
  RDCLOG("Checking that APK can be can write to sdcard");

  string badging = execCommand("aapt dump badging " + apk).strStdout;

  if(badging.find("android.permission.WRITE_EXTERNAL_STORAGE") == string::npos)
  {
    RDCERR("APK missing WRITE_EXTERNAL_STORAGE permission");
    return false;
  }

  if(badging.find("android.permission.INTERNET") == string::npos)
  {
    RDCERR("APK missing INTERNET permission");
    return false;
  }

  return true;
}

bool CheckDebuggable(const string &apk)
{
  RDCLOG("Checking that APK s debuggable");

  string badging = execCommand("aapt dump badging " + apk).strStdout;

  if(badging.find("application-debuggable"))
  {
    RDCERR("APK is not debuggable");
    return false;
  }

  return true;
}
}    // namespace Android

using namespace Android;
extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_GetAndroidFriendlyName(const rdctype::str &device,
                                                                            rdctype::str &friendly)
{
  if(!IsHostADB(device.c_str()))
  {
    RDCERR("Calling RENDERDOC_GetAndroidFriendlyName with non-android device: %s", device.c_str());
    return;
  }

  int index = 0;
  std::string deviceID;
  Android::extractDeviceIDAndIndex(device.c_str(), index, deviceID);

  if(deviceID.empty())
  {
    RDCERR("Failed to get android device and index from: %s", device.c_str());
    return;
  }

  string manuf = trim(adbExecCommand(deviceID, "shell getprop ro.product.manufacturer").strStdout);
  string model = trim(adbExecCommand(deviceID, "shell getprop ro.product.model").strStdout);

  std::string combined;

  if(manuf.empty() && model.empty())
    combined = "";
  else if(manuf.empty() && !model.empty())
    combined = model;
  else if(!manuf.empty() && model.empty())
    combined = manuf + " device";
  else if(!manuf.empty() && !model.empty())
    combined = manuf + " " + model;

  if(combined.empty())
    friendly = "";
  else
    friendly = combined;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_EnumerateAndroidDevices(rdctype::str *deviceList)
{
  string adbStdout = adbGetDeviceList();

  int idx = 0;

  using namespace std;
  istringstream stdoutStream(adbStdout);
  string ret;
  string line;
  while(getline(stdoutStream, line))
  {
    vector<string> tokens;
    split(line, tokens, '\t');
    if(tokens.size() == 2 && trim(tokens[1]) == "device")
    {
      if(ret.length())
        ret += ",";

      ret += StringFormat::Fmt("adb:%d:%s", idx, tokens[0].c_str());

      // Forward the ports so we can see if a remoteserver/captured app is already running.
      adbForwardPorts(idx, tokens[0]);

      idx++;
    }
  }

  *deviceList = ret;
}

bool installRenderDocServer(const string &deviceID)
{
  string targetApk = "RenderDocCmd.apk";
  string serverApk;

  // Check known paths for RenderDoc server
  string exePath;
  FileIO::GetExecutableFilename(exePath);
  string exeDir = dirname(FileIO::GetFullPathname(exePath));

  std::vector<std::string> paths;

#if defined(RENDERDOC_APK_PATH)
  string customPath(RENDERDOC_APK_PATH);
  RDCLOG("Custom APK path: %s", customPath.c_str());

  if(FileIO::IsRelativePath(customPath))
    customPath = exeDir + "/" + customPath;

  // Check to see if APK name was included in custom path
  if(!endswith(customPath, targetApk))
  {
    if(customPath.back() != '/')
      customPath += "/";
    customPath += targetApk;
  }

  paths.push_back(customPath);
#endif

  paths.push_back(exeDir + "/android/apk/" + targetApk);                         // Windows install
  paths.push_back(exeDir + "/../share/renderdoc/android/apk/" + targetApk);      // Linux install
  paths.push_back(exeDir + "/../../build-android/bin/" + targetApk);             // Local build
  paths.push_back(exeDir + "/../../../../../build-android/bin/" + targetApk);    // macOS build

  for(uint32_t i = 0; i < paths.size(); i++)
  {
    RDCLOG("Checking for server APK in %s", paths[i].c_str());
    if(FileIO::exists(paths[i].c_str()))
    {
      serverApk = paths[i];
      RDCLOG("APK found!: %s", serverApk.c_str());
      break;
    }
  }

  if(serverApk.empty())
  {
    RDCERR(
        "%s missing! RenderDoc for Android will not work without it. "
        "Build your Android ABI in build-android in the root to have it "
        "automatically found and installed.",
        targetApk.c_str());
    return false;
  }

  // Build a map so we can switch on the string that returns from adb calls
  enum AndroidAbis
  {
    Android_armeabi,
    Android_armeabi_v7a,
    Android_arm64_v8a,
    Android_x86,
    Android_x86_64,
    Android_mips,
    Android_mips64,
    Android_numAbis
  };

  // clang-format off
  static std::map<std::string, AndroidAbis> abi_string_map;
  abi_string_map["armeabi"]     = Android_armeabi;
  abi_string_map["armeabi-v7a"] = Android_armeabi_v7a;
  abi_string_map["arm64-v8a"]   = Android_arm64_v8a;
  abi_string_map["x86"]         = Android_x86;
  abi_string_map["x86_64"]      = Android_x86_64;
  abi_string_map["mips"]        = Android_mips;
  abi_string_map["mips64"]      = Android_mips64;
  // clang-format on

  // 32-bit server works for 32 and 64 bit apps, so install 32-bit matching ABI
  string adbAbi = trim(adbExecCommand(deviceID, "shell getprop ro.product.cpu.abi").strStdout);

  string adbInstall;
  switch(abi_string_map[adbAbi])
  {
    case Android_armeabi_v7a:
    case Android_arm64_v8a:
      adbInstall = adbExecCommand(deviceID, "install -r --abi armeabi-v7a " + serverApk).strStdout;
      break;
    case Android_armeabi:
    case Android_x86:
    case Android_x86_64:
    case Android_mips:
    case Android_mips64:
    default:
    {
      RDCERR("Unsupported target ABI: %s", adbAbi.c_str());
      return false;
    }
  }

  // Ensure installation succeeded
  string adbCheck =
      adbExecCommand(deviceID, "shell pm list packages org.renderdoc.renderdoccmd").strStdout;
  if(adbCheck.empty())
  {
    RDCERR("Installation of RenderDocCmd.apk failed!");
    return false;
  }

  return true;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_StartAndroidRemoteServer(const char *device)
{
  int index = 0;
  std::string deviceID;

  // legacy code - delete when C# UI is gone. Handle a NULL or empty device string
  if(device || device[0] == '\0')
    Android::extractDeviceIDAndIndex(device, index, deviceID);

  // We should hook up versioning of the server, then re-install if the version is old or mismatched
  // But for now, just install it, if not already present
  string adbPackage =
      adbExecCommand(deviceID, "shell pm list packages org.renderdoc.renderdoccmd").strStdout;
  if(adbPackage.empty())
  {
    if(!installRenderDocServer(deviceID))
      return;
  }

  adbExecCommand(deviceID, "shell am force-stop org.renderdoc.renderdoccmd");
  adbForwardPorts(index, deviceID);
  adbExecCommand(deviceID, "shell setprop debug.vulkan.layers :");
  adbExecCommand(
      deviceID,
      "shell am start -n org.renderdoc.renderdoccmd/.Loader -e renderdoccmd remoteserver");
}

extern "C" RENDERDOC_API bool RENDERDOC_CC RENDERDOC_CheckAndroidLayer(const char *host,
                                                                       const char *exe)
{
  string packageName(basename(string(exe)));

  int index = 0;
  std::string deviceID;
  Android::extractDeviceIDAndIndex(host, index, deviceID);

  // Find the path to package
  string pkgPath = trim(adbExecCommand(deviceID, "shell pm path " + packageName).strStdout);
  pkgPath.erase(pkgPath.begin(), pkgPath.begin() + strlen("package:"));
  pkgPath.erase(pkgPath.end() - strlen("base.apk"), pkgPath.end());
  pkgPath += "lib";

  string layerName = "libVkLayer_GLES_RenderDoc.so";

  // First, see if the application contains the layer
  if(SearchForAndroidLayer(deviceID, pkgPath, layerName))
    return true;

  // Next, check a debug location only usable by rooted devices
  if(SearchForAndroidLayer(deviceID, "/data/local/debug/vulkan", layerName))
    return true;

  // TODO: Add any future layer locations

  RDCWARN("No RenderDoc layer for Vulkan was found");
  return false;
}

string DetermineInstalledABI(const string &deviceID, const string &packageName)
{
  RDCLOG("Checking installed ABI for %s", packageName.c_str());
  string abi;

  string dump = adbExecCommand(deviceID, "shell pm dump " + packageName).strStdout;
  if(dump.empty())
    RDCERR("Unable to pm dump %s", packageName.c_str());

  // Walk through the output and look for primaryCpuAbi
  std::istringstream contents(dump);
  string line;
  string prefix("primaryCpuAbi=");
  while(std::getline(contents, line))
  {
    line = trim(line);
    if(line.compare(0, prefix.size(), prefix) == 0)
    {
      // Extract the abi
      abi = line.substr(line.find_last_of("=") + 1);
      RDCLOG("primaryCpuAbi found: %s", abi.c_str());
      break;
    }
  }

  if(abi.empty())
    RDCERR("Unable to determine installed abi for: %s", packageName.c_str());

  return abi;
}

string FindAndroidLayer(const string &abi, const string &layerName)
{
  string layer;

  // Check known paths for RenderDoc layer
  string exePath;
  FileIO::GetExecutableFilename(exePath);
  string exeDir = dirname(FileIO::GetFullPathname(exePath));

  std::vector<std::string> paths;

#if defined(RENDERDOC_LAYER_PATH)
  string customPath(RENDERDOC_LAYER_PATH);
  RDCLOG("Custom layer path: %s", customPath.c_str());

  if(FileIO::IsRelativePath(customPath))
    customPath = exeDir + "/" + customPath;

  // Check to see if layer name was included in custom path
  if(!endswith(customPath, layerName))
  {
    if(customPath.back() != '/')
      customPath += "/";
    customPath += layerName;
  }
  paths.push_back(customPath);
#endif

  // clang-format off
  paths.push_back(exeDir + "/android/libs/" + abi + "/" + layerName);                                   // Windows install
  paths.push_back(exeDir + "/../share/renderdoc/android/libs/" + abi + "/" + layerName);                // Linux install
  paths.push_back(exeDir + "/../../build-android/renderdoccmd/libs/" + abi + "/" + layerName);          // Local build
  paths.push_back(exeDir + "/../../../../../build-android/renderdoccmd/libs/" + abi + "/" + layerName); // macOS build
  // clang-format on

  for(uint32_t i = 0; i < paths.size(); i++)
  {
    RDCLOG("Checking for layer in %s", paths[i].c_str());
    if(FileIO::exists(paths[i].c_str()))
    {
      layer = paths[i];
      RDCLOG("Layer found!: %s", layer.c_str());
      break;
    }
  }

  if(layer.empty())
  {
    RDCERR(
        "%s missing! RenderDoc for Android will not work without it. "
        "Build your Android ABI in build-android in the root to have it "
        "automatically found and installed.",
        layer.c_str());
  }

  return layer;
}

extern "C" RENDERDOC_API bool RENDERDOC_CC RENDERDOC_AddLayerToAndroidPackage(const char *host,
                                                                              const char *exe,
                                                                              float *progress)
{
  Process::ProcessResult result = {};
  string packageName(basename(string(exe)));

  int index = 0;
  std::string deviceID;
  Android::extractDeviceIDAndIndex(host, index, deviceID);

  *progress = 0.0f;

  if(!CheckPatchingRequirements())
    return false;

  *progress = 0.11f;

  // Detect which ABI was installed on the device
  string abi = DetermineInstalledABI(deviceID, packageName);

  // Find the layer on host
  string layerName("libVkLayer_GLES_RenderDoc.so");
  string layerPath = FindAndroidLayer(abi, layerName);
  if(layerPath.empty())
    return false;

  // Find the APK on the device
  string pkgPath = trim(adbExecCommand(deviceID, "shell pm path " + packageName).strStdout);
  pkgPath.erase(pkgPath.begin(), pkgPath.begin() + strlen("package:"));

  string tmpDir = FileIO::GetTempFolderFilename();
  string origAPK(tmpDir + packageName + ".orig.apk");
  string alignedAPK(origAPK + ".aligned.apk");

  *progress = 0.21f;

  // Try the following steps, bailing if anything fails
  if(!PullAPK(deviceID, pkgPath, origAPK))
    return false;

  *progress = 0.31f;

  if(!CheckPermissions(origAPK))
    return false;

  *progress = 0.41f;

  if(!RemoveAPKSignature(origAPK))
    return false;

  *progress = 0.51f;

  if(!AddLayerToAPK(origAPK, layerPath, layerName, abi, tmpDir))
    return false;

  *progress = 0.61f;

  if(!RealignAPK(origAPK, alignedAPK, tmpDir))
    return false;

  *progress = 0.71f;

  if(!DebugSignAPK(alignedAPK, tmpDir))
    return false;

  *progress = 0.81f;

  if(!UninstallOriginalAPK(deviceID, packageName, tmpDir))
    return false;

  *progress = 0.91f;

  if(!ReinstallPatchedAPK(deviceID, alignedAPK, abi, packageName, tmpDir))
    return false;

  *progress = 1.0f;

  // All clean!
  return true;
}

extern "C" RENDERDOC_API bool RENDERDOC_CC RENDERDOC_NeedVulkanLayerRegistration(
    VulkanLayerFlags *flagsPtr, rdctype::array<rdctype::str> *myJSONsPtr,
    rdctype::array<rdctype::str> *otherJSONsPtr)
{
  VulkanLayerFlags flags = VulkanLayerFlags::NoFlags;
  std::vector<std::string> myJSONs;
  std::vector<std::string> otherJSONs;

  bool ret = RenderDoc::Inst().NeedVulkanLayerRegistration(flags, myJSONs, otherJSONs);

  if(flagsPtr)
    *flagsPtr = flags;

  if(myJSONsPtr)
  {
    create_array(*myJSONsPtr, myJSONs.size());
    for(size_t i = 0; i < myJSONs.size(); i++)
      (*myJSONsPtr)[i] = myJSONs[i];
  }

  if(otherJSONsPtr)
  {
    create_array(*otherJSONsPtr, otherJSONs.size());
    for(size_t i = 0; i < otherJSONs.size(); i++)
      (*otherJSONsPtr)[i] = otherJSONs[i];
  }

  return ret;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_UpdateVulkanLayerRegistration(bool systemLevel)
{
  RenderDoc::Inst().UpdateVulkanLayerRegistration(systemLevel);
}
