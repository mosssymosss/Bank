Прозрачный банк хранит N счетов с номерами от 0 до N-1. У каждого счета имеется:
* текущий баланс -- целое знаковое число (баланс может быть отрицательным)
* минимальный возможный баланс -- целое знаковое число
* максимальный возможный баланс -- целое знаковое число
* заморожен ли счет, то есть позволяется ли менять текущий баланс
Должно выполняться условие:
минимальный возможный остаток <= текущий баланс <= максимальный возможный остаток.

Программа-инициализатор создает новый банк с N (аргумент командной строки) не замороженными счетами с нулевым балансом, нулевым минимальным остатком и некоторым положительным максимальным возможным остатком.

Программа-деинициализатор уничтожает существующий банк.

Программа-клиент читает со стандартного ввода запросы и исполняет их. Запрос - одна строка из потока ввода. Так банк абсолютно прозрачный, любой клиент может выполнять любые допустимые операции над любыми счетами.
Запросы бывают следующего вида:
вывести на экран текущий/минимальный/максимальный баланс счета с заданным номером A
заморозить/разморозить счет с заданным номером A
перевести сумму X со счета A на счет B, X > 0
зачислить на все счета / списать со всех счетов по X единиц
установить заданный минимальный/максимальный возможный остаток X для счета с заданным номером A
В случае невозможности выполнения операций (например, заморожен счет, операция приведет к выходу за допустимый кредитный лимит, некорректный номер счета, некорректный ввод данных и пр.) программа вежливо выводит сообщение об этом и читает следующий запрос. В случае успешной транзакции вывести сообщение об этом. Формат запросов придумайте самостоятельно.

Банк хранится в разделяемой памяти, работа с банком не должна нарушать целостность данных. В частности, при переводе средств с одного счета на другой не может быть ситуации, что у кого-то средства списались, но не зачислились на другой счет, или деньги списались с замороженного счета.

Подготовьте набор тестов для проверки корректности работы.

Makefile содержит следующие цели с корректно описанными зависимостями:
для сборки каждого объектного модуля (.o)
для release сборки (без включения отладочной информации)
для отладочной сборки и запуска тестов с valgrind (memcheck, helgrind), с проверкой правильности результата
для сборки с целью измерения покрытия на тестовом наборе

Язык для реализации: C/C++.