@echo off
set SolutionDir=%1
set Configuration=%2
if "%SolutionDir%"=="" (
  echo Missing SolutionDir
  goto END
)
if "%Configuration%"=="" (
  echo Missing Configuration
  goto END
)
echo SolutionDir=%SolutionDir%
echo Configuration=%Configuration%

echo d|xcopy %SolutionDir%%Configuration% %SolutionDir%out\%Configuration% /e /d /exclude:%SolutionDir%exclude.list
echo n|copy /-y %SolutionDir%Resources\cef.pak %SolutionDir%out\%Configuration%
echo n|copy /-y %SolutionDir%Resources\devtools_resources.pak %SolutionDir%out\%Configuration%
mkdir %SolutionDir%out\%Configuration%\locales
echo n|copy /-y %SolutionDir%Resources\locales\en-US.pak %SolutionDir%out\%Configuration%\locales
:END
rem @pause>nul