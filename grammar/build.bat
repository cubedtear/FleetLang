@echo off
java -jar antlr4.jar -Dlanguage=Cpp -no-listener -o ../gen -visitor FleetLang.g4
echo ------ Grammar updated