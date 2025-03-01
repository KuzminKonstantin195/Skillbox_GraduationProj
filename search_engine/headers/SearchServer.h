#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "InvertedIndex.h"

#pragma once

struct RelativeIndex {
	size_t doc_id;
	float rank;

	RelativeIndex() {};
	RelativeIndex(rsize_t id, float rn) : doc_id(id), rank(rn) {};

	bool operator ==(const RelativeIndex& other) const {
		return (doc_id == other.doc_id && rank == other.rank);
	}

	RelativeIndex& operator =(const Entry& entr)
	{
		doc_id = entr.doc_id;
		rank = entr.count;
		return *this;
	}
};

class SearchServer {
public:
	/**
	* @param idx в конструктор класса передаётся ссылка на класс
	InvertedIndex,
	* чтобы SearchServer мог узнать частоту слов встречаемых в
	запросе
	*/
	SearchServer(InvertedIndex& idx) : _index(idx) {};
	/**
	* Метод обработки поисковых запросов
	* @param queries_input поисковые запросы взятые из файла
	requests.json
	* @return возвращает отсортированный список релевантных ответов для
	заданных запросов
	*/
	std::vector<std::vector<RelativeIndex>> search (const
		std::vector<std::string>& queries_input);
private:
	InvertedIndex _index;
	
	std::vector<std::string> wordSeparate (const std::string& line);
	std::string reformatingWord(std::string word);									// форматирование слов
	std::vector<RelativeIndex> searchOneQuery(const std::vector<std::string>& words);
	std::pair<int,int> findMax(std::map<int, int> dict);							// возвращает ключ с самым большим значением
};