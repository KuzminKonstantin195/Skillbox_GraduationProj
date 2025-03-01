#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <queue>

#include <thread>
#include <mutex>
#include <memory>

#pragma once


struct Entry {
	size_t doc_id, count;
	
	Entry() : doc_id(0), count(0) {};
	Entry(size_t num, size_t _count) : doc_id(num), count(_count) {};

	// Данный оператор необходим для проведения тестовых сценариев
	bool operator ==(const Entry& other) const
	{
		return (doc_id == other.doc_id &&
			count == other.count);
	}

	Entry& operator =(const Entry& other)
	{
		doc_id = other.doc_id;
		count = other.count;

		return *this;
	}
};

class InvertedIndex {
	
public:

	InvertedIndex() = default;
	/**
	* Обновить или заполнить базу документов, по которой будем совершать
	поиск
	* @param texts_input содержимое документов
	*/
	void UpdateDocumentBase(std::vector<std::string> input_docs);
	/**
	* Метод определяет количество вхождений слова word в загруженной базе
документов
	* @param word слово, частоту вхождений которого необходимо определить
	* @return возвращает подготовленный список с частотой слов
	*/
	std::vector<Entry> GetWordCount(const std::string& word);
	
private:
	std::vector<std::map<std::string, Entry>> subDictionariesBuffer;

	std::vector<std::string> docs;									// список содержимого документов
	std::map<std::string, std::vector<Entry>> freq_dictionary;		// частотный словарь

	void makeIndexFromText(std::string text, int textNum);			// индексирование слов в потоке
	std::string reformatingWord(std::string word);					// форматирование слов
	std::map<std::string, Entry> Indexing (std::string word, int textNum);

	void dictionariesMerge (std::vector<std::map<std::string, Entry>>& subDicts);
};
