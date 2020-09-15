for /r %%F in (*.cpp *.h) DO (
  echo Working on %%F
  copy _license.txt newFile.tmp
  type "%%~F" >> newFile.tmp
  del "%%~F"
  copy newFile.tmp "%%~F"
  del newFile.tmp
)