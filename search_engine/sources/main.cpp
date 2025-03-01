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
	SearchServer server (invIndex);

	auto texts = converter.GetTextDocuments();

	auto queryes = converter.GetRequests();

	invIndex.UpdateDocumentBase(texts);
	
	auto answers = server.search(texts);

	vector<vector<pair<int, float>>> toPutAnswers;

	toPutAnswers = rebuildAnswersToPut(answers);

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