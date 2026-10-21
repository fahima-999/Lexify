@echo off
cd /d "%~dp0"
javac *.java
if errorlevel 1 exit /b 1
java Main input.java.txt
pause

