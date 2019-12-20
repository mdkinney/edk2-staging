/**

Implement UnitTestLib 

Copyright (c) Microsoft
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <Library/UnitTestLib.h>
#include <UnitTestFrameworkTypes.h>
#include <Library/UnitTestLogLib.h>


STATIC
EFI_STATUS
AddUnitTestFailure(
  IN OUT UNIT_TEST    *UnitTest,
  IN CONST CHAR8      *FailureMessage,
  IN FAILURE_TYPE     FailureType
  )
{
  //
  // Make sure that you're cooking with gas.
  //
  if (UnitTest == NULL || FailureMessage == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  UnitTest->FailureType = FailureType;
  AsciiStrCpyS(&UnitTest->FailureMessage[0], UNIT_TEST_TESTFAILUREMSG_LENGTH, FailureMessage);

  return EFI_SUCCESS;
} // AddUnitTestFailure()


STATIC
VOID
UnitTestLogFailure(
  IN  UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN  FAILURE_TYPE                FailureType,
  IN  CONST CHAR8                 *Format,
  ...
  )
{
  CHAR8         LogString[UNIT_TEST_TESTFAILUREMSG_LENGTH];
  VA_LIST       Marker;


  //
  // Convert the message to an ASCII String
  //
  VA_START(Marker, Format);
  AsciiVSPrint(LogString, sizeof(LogString), Format, Marker);
  VA_END(Marker);

  //
  // Finally, add the string to the log.
  //
  AddUnitTestFailure(((UNIT_TEST_FRAMEWORK*)Framework)->CurrentTest, LogString, FailureType);

  return;
} // UnitTestLogFailure()


BOOLEAN
EFIAPI
UnitTestAssertTrue (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN BOOLEAN                    Expression,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *Description
  )
{
  if (!Expression)
  {
    UnitTestLogFailure(Framework, FAILURETYPE_ASSERTTRUE, "%a::%d Expression (%a) is not TRUE!\n", FunctionName, LineNumber, Description);
    UnitTestLog(Framework, DEBUG_ERROR, "[ASSERT FAIL] %a::%d Expression (%a) is not TRUE!\n", FunctionName, LineNumber, Description );
  }
  return Expression;
}


BOOLEAN
EFIAPI
UnitTestAssertFalse (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN BOOLEAN                    Expression,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *Description
  )
{
  if (Expression)
  {
    UnitTestLogFailure(Framework, FAILURETYPE_ASSERTFALSE, "%a::%d Expression(%a) is not FALSE!\n", FunctionName, LineNumber, Description );
    UnitTestLog(Framework, DEBUG_ERROR,"[ASSERT FAIL] %a::%d Expression (%a) is not FALSE!\n", FunctionName, LineNumber, Description );
  }
  return !Expression;
}


BOOLEAN
EFIAPI
UnitTestAssertNotEfiError (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN EFI_STATUS                 Status,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *Description
  )
{
  if (EFI_ERROR( Status ))
  {
    UnitTestLogFailure(Framework, FAILURETYPE_ASSERTNOTEFIERROR, "%a::%d Status '%a' is EFI_ERROR (%r)!\n", FunctionName, LineNumber, Description, Status);
    UnitTestLog(Framework, DEBUG_ERROR,"[ASSERT FAIL] %a::%d Status '%a' is EFI_ERROR (%r)!\n", FunctionName, LineNumber, Description, Status );
  }
  return !EFI_ERROR( Status );
}


BOOLEAN
EFIAPI
UnitTestAssertEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UINT64                     ValueA,
  IN UINT64                     ValueB,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *DescriptionA,
  IN CONST CHAR8                *DescriptionB
  )
{
  if ((ValueA != ValueB))
  {
    UnitTestLogFailure(Framework, FAILURETYPE_ASSERTEQUAL, "%a::%d Value %a != %a (%d != %d)!\n", FunctionName, LineNumber, DescriptionA, DescriptionB, ValueA, ValueB);
    UnitTestLog(Framework, DEBUG_ERROR,"[ASSERT FAIL] %a::%d Value %a != %a (%d != %d)!\n", FunctionName, LineNumber, DescriptionA, DescriptionB, ValueA, ValueB );
  }
  return (ValueA == ValueB);
}

BOOLEAN
EFIAPI
UnitTestAssertMemEqual(
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UINTN                      ValueA,
  IN UINTN                      ValueB,
  IN UINTN                      Length,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *DescriptionA,
  IN CONST CHAR8                *DescriptionB
  )
{
  if (CompareMem((VOID*)ValueA, (VOID*)ValueB, Length) != 0)
  {
    UnitTestLogFailure(Framework, FAILURETYPE_ASSERTEQUAL, __FUNCTION__, "%a::%d Memory at %a != %a for length %d bytes!\n", FunctionName, LineNumber, DescriptionA, DescriptionB, Length);
    UnitTestLog(Framework, DEBUG_ERROR, "[ASSERT FAIL] %a::%d Value %a != %a for length %d bytes!\n", FunctionName, LineNumber, DescriptionA, DescriptionB, Length);
    return FALSE;
  }
  return TRUE;
}


BOOLEAN
EFIAPI
UnitTestAssertNotEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN UINT64                     ValueA,
  IN UINT64                     ValueB,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *DescriptionA,
  IN CONST CHAR8                *DescriptionB
  )
{
  if ((ValueA == ValueB))
  {
    UnitTestLogFailure(Framework, FAILURETYPE_ASSERTNOTEQUAL,"%a::%d Value %a == %a (%d == %d)!\n", FunctionName, LineNumber, DescriptionA, DescriptionB, ValueA, ValueB);
    UnitTestLog(Framework, DEBUG_ERROR,"[ASSERT FAIL] %a::%d Value %a == %a (%d == %d)!\n", FunctionName, LineNumber,DescriptionA, DescriptionB, ValueA, ValueB );
  }
  return (ValueA != ValueB);
}


BOOLEAN
EFIAPI
UnitTestAssertStatusEqual (
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN EFI_STATUS                 Status,
  IN EFI_STATUS                 Expected,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *Description
  )
{
  if ((Status != Expected))
  {
    UnitTestLogFailure(Framework, FAILURETYPE_ASSERTSTATUSEQUAL, "%a::%d Status '%a' is %r, should be %r!\n", FunctionName, LineNumber, Description, Status, Expected);
    UnitTestLog(Framework, DEBUG_ERROR,"[ASSERT FAIL] %a::%d Status '%a' is %r, should be %r!\n", FunctionName, LineNumber, Description, Status, Expected );
  }
  return (Status == Expected);
}

BOOLEAN
EFIAPI
UnitTestAssertNotNull(
  IN UNIT_TEST_FRAMEWORK_HANDLE Framework,
  IN VOID*                      Pointer,
  IN CONST CHAR8                *FunctionName,
  IN UINTN                      LineNumber,
  IN CONST CHAR8                *FileName,
  IN CONST CHAR8                *PointerName
  )
{
  if (Pointer == NULL)
  {
    UnitTestLogFailure(Framework, FAILURETYPE_ASSERTNOTNULL, "%a::%d Pointer (%a) is NULL!\n", FunctionName, LineNumber, PointerName);
    UnitTestLog(Framework, DEBUG_ERROR, "[ASSERT FAIL] %a::%d Pointer (%a) is NULL!\n", FunctionName, LineNumber, PointerName);
  }
  return (Pointer != NULL);

}
