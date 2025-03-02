#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

using namespace std;

vector<vector<pair<int, float>>> rebuildAnswersToPut (vector<vector<RelativeIndex>> &answers);

int main()
{
	setlocale(LC_ALL, "Russiаn");
	cout << "##################################################################################!" << endl;

	cout << "\t\t\tСтарт!" << endl;

	ConverterJSON converter;
	InvertedIndex invIndex;

	auto texts = converter.GetTextDocuments();
	auto queryes = converter.GetRequests();
	invIndex.UpdateDocumentBase(texts);
	
	SearchServer server(invIndex);
	server.maxAnswerCount = converter.GetResponsesLimit();		// задаем лимит ответов
	auto answers = server.search(queryes);

	vector<vector<pair<int, float>>> toPutAnswers;

	toPutAnswers = rebuildAnswersToPut(answers);
	cout << "Answers: " << endl;

	for (auto& i : toPutAnswers)
	{
		cout << "\t";
		for (auto& ii : i)
		{
			cout << ii.first << ": " << ii.second << " ";
		}
		cout << endl;
	}
	converter.putAnswers(toPutAnswers);

	cout << "Завершение!" << endl;
	cout << "##################################################################################!" << endl;

}

vector<vector<pair<int, float>>> rebuildAnswersToPut(vector<vector<RelativeIndex>> &answers)
{
	vector<vector<pair<int, float>>> result;

	for (auto& vec : answers)
	{
		vector<pair<int, float>> subResult;
		for (auto& i : vec)
		{
			subResult.push_back(pair<int, float>(i.doc_id, i.rank));
		}
		result.push_back(subResult);
	}
	return result;


}