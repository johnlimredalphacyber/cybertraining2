#include "backend.h"
#include "gxa.h"
#include "targets.h"
#include "crash_detection_umode.h"
#include <cstdint>
#include <cstdlib>
#include <fmt/format.h>

namespace ac_server {




struct {
  CpuState_t Context;

  void RestoreGprs(Backend_t *B) {
    const auto &C = Context;
    B->Rsp(C.Rsp);
    B->Rip(C.Rip);
    B->Rax(C.Rax);
    B->Rbx(C.Rbx);
    B->Rcx(C.Rcx);
    B->Rdx(C.Rdx);
    B->Rsi(C.Rsi);
    B->Rdi(C.Rdi);
    B->R8(C.R8);
    B->R9(C.R9);
    B->R10(C.R10);
    B->R11(C.R11);
    B->R12(C.R12);
    B->R13(C.R13);
    B->R14(C.R14);
    B->R15(C.R15);
  }
} GlobalState;
int Number_Packets=10, PassedPackets=0;
constexpr bool LoggingOn = false;

template <typename... Args_t>
void DebugPrint(const char *Format, const Args_t &...args) {
  if constexpr (LoggingOn) {
    fmt::print("Ac_Server: ");
    fmt::print(fmt::runtime(Format), args...);
  }
}
#define MAX_SIZE 0x50
bool InsertTestcase(const uint8_t *Buffer, const size_t BufferSize) {
  if (BufferSize > 0x1000) {
    return false;
  }

  const Gva_t Rsp = Gva_t(g_Backend->GetReg(Registers_t::Rsp));


  Gva_t ptr_packet = Rsp+Gva_t(0x20);

  Gva_t packet_buf_ptr = Gva_t(g_Backend->VirtRead8(ptr_packet) & 0xFFFFFFF);

  if (!g_Backend->VirtWriteDirty(packet_buf_ptr, Buffer,MAX_SIZE)) {
    DebugPrint("Failed to write next testcase!");
    return false;
  }

  // g_Backend->SetReg(Registers_t::Rax, static_cast<char>((random() >> 6) & 0xFF));
  
  

  return true;

}

bool Init(const Options_t &Opts, const CpuState_t &State) {
    GlobalState.Context = State;
    if (!g_Backend->SetBreakpoint(Gva_t(0x0043fe8b), [](Backend_t *Backend) {
        PassedPackets++;
        //DebugPrint("PassedPackets ==  {} , {}\n", PassedPackets,g_Backend->GetReg(Registers_t::Rax));
        if(Number_Packets > PassedPackets){
        //DebugPrint("Rerun\n");
        //PassedPackets = 0;
         GlobalState.RestoreGprs(g_Backend);


         
         }else {
         PassedPackets = 0;
         }
      })) {
    DebugPrint("Failed to SetBreakpoint EndProcess\n");
    return false;
  }
  

  
  if (!g_Backend->SetBreakpoint(Gva_t(0x004361b1), [](Backend_t *Backend) {
         //Backend->Stop(Ok_t());
         DebugPrint("Bp 0 Rax == {}\n",g_Backend->GetReg(Registers_t::Rax));
         DebugPrint("\n");
      
      })) {
    DebugPrint("Failed to SetBreakpoint CheckStack failed\n");
    return false;
  }

          if (!g_Backend->SetBreakpoint(Gva_t(0x004361b8), [](Backend_t *Backend) {
         //Backend->Stop(Ok_t());
         DebugPrint("Bp 0.5 Rax == {}\n",g_Backend->GetReg(Registers_t::Rax));
         DebugPrint("\n");
      
      })) {
    DebugPrint("Failed to SetBreakpoint CheckStack failed\n");
    return false;
  }

      if (!g_Backend->SetBreakpoint(Gva_t(0x004361fe), [](Backend_t *Backend) {
         //Backend->Stop(Ok_t());
         DebugPrint("Bp 1\n");
      
      })) {
    DebugPrint("Failed to SetBreakpoint CheckStack failed\n");
    return false;
  }
    if (!g_Backend->SetBreakpoint(Gva_t(0x0043724e), [](Backend_t *Backend) {
         //Backend->Stop(Ok_t());
               DebugPrint("Bp 2\n");

      })) {
    DebugPrint("Failed to SetBreakpoint CheckStack failedà\n");
    return false;
  }


  
  if (!g_Backend->SetBreakpoint(Gva_t(0x00435946), [](Backend_t *Backend) {
         //Backend->Stop(Ok_t());
         DebugPrint("SV primary weapon\n");
         g_Backend->PrintRegisters();

      })) {
    DebugPrint("Failed to SetBreakpoint CheckStack failed\n");
    return false;
  }
    
  if (!g_Backend->SetBreakpoint(Gva_t(0x00435959), [](Backend_t *Backend) {
         //Backend->Stop(Ok_t());
         DebugPrint("SV primary weapon\n");

        

        const Gva_t Rsi = Gva_t(g_Backend->GetReg(Registers_t::Rsi));

        Gva_t PrimaryWeapon = Gva_t(g_Backend->VirtRead8(Rsi+Gva_t(0x288)));

        DebugPrint("Primary weapon = {:x} \n", PrimaryWeapon);
              

      })) {
    DebugPrint("Failed to SetBreakpoint CheckStack failed\n");
    return false;
  }
  

  if (!g_Backend->SetBreakpoint(Gva_t(0x00401a5e), [](Backend_t *Backend) -> void {

     // Backend->Stop(Ok_t());
     // const Gva_t Rsi = Gva_t(Backend->GetReg(Registers_t::Rcx) + 0x0C);

      // const uint8_t buf[] = {0x44,0x44,0x44,0x44,0x44, };

      // size_t bufferSize = sizeof(buf); 

      // if (!Backend->VirtWriteDirty(Rsi, buf, bufferSize)) {
      //     DebugPrint("Failed to write next testcase!");
      // }

      // Backend->SetReg(Registers_t::Rdx, 0x444444);

      // Backend->SetReg(Registers_t::Rdi, 0x444444);

     // g_Backend->SetReg(Registers_t::Rip, 0x0);


        //Gva_t PrimaryWeapon = Gva_t(g_Backend->VirtRead8(Rsi));

        //DebugPrint("Primary weapon = {:x} \n", PrimaryWeapon);


    })) {
    DebugPrint("Failed to SetBreakpoint CheckStack failed\n");
    return false;
  }

  if (!g_Backend->SetBreakpoint(Gva_t(0x00437271), [](Backend_t *Backend) {
         //Backend->Stop(Ok_t());
         DebugPrint("Bp 2.2 \n");
      
      })) {
    DebugPrint("Failed to SetBreakpoint CheckStack failed\n");
    return false;
  }




 SetupUsermodeCrashDetectionHooks();





  return true;
}

bool Restore() { return true; }


Target_t ac_server("ac_server", Init, InsertTestcase,Restore);

} // namespace ac_server
