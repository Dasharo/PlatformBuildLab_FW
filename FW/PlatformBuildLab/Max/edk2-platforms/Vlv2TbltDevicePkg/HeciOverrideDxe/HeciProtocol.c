
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>

#include "HeciProtocol.h"

#if defined(SEC_DEBUG_INFO) && SEC_DEBUG_INFO
#define PRINT(x...) DEBUG ((EFI_D_INFO, x))
#else
#define PRINT(x...) Print (L ## x)
#endif

EFI_HECI_PROTOCOL mHeciProtocolOld;
EFI_HECI_PROTOCOL mHeciProtocolNew;

EFI_STATUS
EFIAPI HeciSendWithAck (
  IN OUT  UINT32           *Message,
  IN OUT  UINT32           Length,
  IN OUT  UINT32           *RecLength,
  IN      UINT8            HostAddress,
  IN      UINT8            SECAddress
  )
{
  EFI_STATUS Status;
  UINT8 *Buf = (UINT8 *)Message;

  PRINT ("HeciSendWithAck: Length %x, RecLength %s, HostAddr %x, SeCAddr %x\n", Length, *RecLength, HostAddress, SECAddress);
  PRINT ("HeciSendWithAck: Message:\n");

  for (UINT32 i = 0; i < Length; i++)
    PRINT ("%02x ", Buf[i]);

  PRINT ("\n");

  Status = mHeciProtocolOld.SendwACK(Message, Length, RecLength, HostAddress, SECAddress);

  if (!EFI_ERROR (Status)) {

    PRINT ("HeciSendWithAck: Response:\n");

    Buf = (UINT8 *)Message;

    for (UINT32 i = 0; i < *RecLength; i++)
      PRINT ("%02x ", Buf[i]);

    PRINT ("\n");
  } else {
    PRINT ("HECI SendwACK failed %r\n", Status);
  }

  return Status;
}

EFI_STATUS
EFIAPI HeciSend (
  IN      UINT32           *Message,
  IN      UINT32           Length,
  IN      UINT8            HostAddress,
  IN      UINT8            SECAddress
  )
{
  EFI_STATUS Status;
  UINT8 *Buf = (UINT8 *)Message;

  PRINT ("HeciSend: Length %x, HostAddr %x, SeCAddr %x\n", Length, HostAddress, SECAddress);
  PRINT ("HeciSend: Message:\n");

  for (UINT32 i = 0; i < Length; i++)
    PRINT ("%02x ", Buf[i]);

  PRINT ("\n");

  Status = mHeciProtocolOld.SendMsg(Message, Length, HostAddress, SECAddress);

  if (EFI_ERROR (Status)) {
    PRINT ("HECI SendMsg failed %r\n", Status);
  }

  return Status;
}

EFI_STATUS
EFIAPI HeciReceive (
  IN      UINT32           Blocking,
  IN      UINT32           *MessageBody,
  IN OUT  UINT32           *Length
  )
{
  EFI_STATUS Status;
  UINT8 *Buf;

  PRINT ("HeciReceive: Length %x, Blocking %x\n", *Length, Blocking);

  Status = mHeciProtocolOld.ReadMsg(Blocking, MessageBody, Length);

  if (!EFI_ERROR (Status)) {

    PRINT ("HeciReceive: Response:\n");

    Buf = (UINT8 *)MessageBody;

    for (UINT32 i = 0; i < *Length; i++)
      PRINT ("%02x ", Buf[i]);

    PRINT ("\n");
  } else {
    PRINT ("HECI Receive failed %r\n", Status);
  }

  return Status;
}


EFI_STATUS
EFIAPI
ReinstallHeciProtocol(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HECI_PROTOCOL                 *ProtocolOld;
  EFI_HANDLE                        HeciHandle;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             BufferSize = 0;


  DEBUG ((EFI_D_INFO, ">>>>ReinstallHeciProtocol\n"));

  //
  // Run memory test code at this point.
  //
  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &ProtocolOld
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "gEfiHeciProtocolGuid not found: %r\n", Status));
    return Status;
  }

  gBS->CopyMem((VOID *)&mHeciProtocolOld, (VOID *)ProtocolOld, sizeof(EFI_HECI_PROTOCOL));

  BufferSize = sizeof (EFI_HANDLE);
  HandleBuffer = AllocateZeroPool (BufferSize);

  Status = gBS->LocateHandle(
                  ByProtocol,
                  &gEfiHeciProtocolGuid,
                  NULL,
                  &BufferSize,
                  HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "gEfiHeciProtocolGuid handles not found %r\n", Status));
    return Status;
  }

  if (BufferSize > sizeof (EFI_HANDLE)) {
    DEBUG ((EFI_D_INFO, "gEfiHeciProtocolGuid handles found %d\n", BufferSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  HeciHandle = *HandleBuffer;
  FreePool (HandleBuffer);

  mHeciProtocolNew.SendwACK = HeciSendWithAck;
  mHeciProtocolNew.ReadMsg = HeciReceive;
  mHeciProtocolNew.SendMsg = HeciSend;
  mHeciProtocolNew.ResetHeci = mHeciProtocolOld.ResetHeci;
  mHeciProtocolNew.InitHeci = mHeciProtocolOld.InitHeci;
  mHeciProtocolNew.SeCResetWait = mHeciProtocolOld.SeCResetWait;
  mHeciProtocolNew.ReInitHeci = mHeciProtocolOld.ReInitHeci;
  mHeciProtocolNew.GetSeCStatus = mHeciProtocolOld.GetSeCStatus;
  mHeciProtocolNew.GetSeCMode = mHeciProtocolOld.GetSeCMode;


  Status = gBS->ReinstallProtocolInterface (
                  HeciHandle,
                  &gEfiHeciProtocolGuid,
                  ProtocolOld,
                  &mHeciProtocolNew
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Could not reinstall gEfiHeciProtocolGuid: %r\n", Status));
    return Status;
  }

  DEBUG ((EFI_D_INFO, "gEfiHeciProtocolGuid reinstalled successfully\n"));

  return Status;
}
