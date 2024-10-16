
/**
 * @brief 
 * 1. Получает списки бинарных пакетов 2-х веток
 * 2. делает сравнение полученных списков пакетов и выводит JSON, в котором будет отображено:
 *  - все пакеты, которые есть в 1-й но нет во 2-й
 *  - все пакеты, которые есть в 2-й но их нет в 1-й
 *  - все пакеты, version-release которых больше в 1-й чем во 2-й
 * 
 * @param branch1 имя ветки 1
 * @param branch2 имя ветки 2
 * @return int 0 в случае успеха, <0 в случае ошибки
 */
int diff_branch_pack(const char *branch1, const char *branch2);