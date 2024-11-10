# Запуск
1. Скачать и установить https://github.com/OpenCppCoverage/OpenCppCoverage/releases
2. Открыть папку с решением в Visual Studio и запустить .sln файл. Должна отобразиться консоль с результатами тестов.
3. В командной строке от имени администратора запустить:


```$ "your\path\to\OpenCppCoverage.exe" --export_type=html:your\path\to\gvozd_aho\coverage_report --sources="your\path\to\gvozd_aho\aho_corasick\aho_corasick\*" -- "your\path\to\gvozd_aho\aho_corasick\x64\Debug\aho_corasick.exe"```

Отчет появится в папке coverage_report. Откройте .html файл, чтобы увидеть подробности.

# Примечание
1. В ветке mc-dc попытался изменить тесты так, чтобы добавить report по покрытию MC/DC, но после этого OpenCppCoverage стал показывать 99% покрытия вместо 100% :(
2. Все тесты в файле test_main.cpp.
