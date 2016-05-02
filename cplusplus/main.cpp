/*
см. README.md
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <iterator>
#include <iomanip>

using namespace std;

class Graph
{
	using size_type = unsigned long;
	using value_type = bool;
	using iterator = vector<bool>::iterator;
	using const_iterator = vector<bool>::const_iterator;

	vector<bool> _vertexs;
	vector<string> _names;
	size_type _row_size = 0;
	bool _state = false;

	int _fill_map(const char* filename, unordered_map<string, int>& names)
	{
		ifstream input(filename);
		string value;

		if (!input.good())
			return 1;

		for (auto i = 0; input >> value; ++i)
			if (!names.insert(make_pair(value, i)).second)
				return 1;
		_row_size = names.size();
		return 0;
	}
	int _fill_vertexs(const char* filename, unordered_map<string, int>& names)
	{
		ifstream input(filename);
		string value;

		if (!input.good())
			return 1;
		_vertexs.resize(_row_size*_row_size);
		auto row = 0;
		auto temp_row = 0;
		stringstream ss;
		try {
			while (input >> value)
			{
				row = names.at(value);
				getline(input, value);
				ss.str(value);
				while (ss >> value)
				{
					temp_row = names.at(value);
					if (row != temp_row)
						_vertexs[temp_row * _row_size + row] = true;
				}
				ss.clear();
			}
		}
		catch (const out_of_range& e)
		{
			cerr << e.what() << endl;
			_vertexs.clear();
			_row_size = 0;
			return 1;
		}
		return 0;
	}

public:
	Graph() = default;
	Graph(const Graph& graph) = default;
	Graph(Graph&& graph) = default;
	Graph(const char* name_of_vertex, const char* adjacency_relation)
	{
		fill_graph(name_of_vertex, adjacency_relation);
	}

	iterator begin() { return _vertexs.begin(); }
	const_iterator begin() const { return _vertexs.cbegin(); }
	iterator end() { return _vertexs.end(); }
	const_iterator end() const { return _vertexs.cend(); }

	size_type size() const { return _vertexs.size(); }
	bool good() const { return _state; }
	void clear()
	{
		_vertexs.clear();
		_names.clear();
		_row_size = 0;
		_state = false;
	}
	bool is_someone_likable(int index) const
	{
		for (auto i = size_type(0); i < _row_size; ++i)
			if (_vertexs[i*_row_size + index] != false)
				return true;
		return false;
	}
	int fill_graph(const char* name_of_vertex, const char* adjacency_relation)
	{
		clear();
		unordered_map<string, int> names;
		//this first
		if (_fill_map(name_of_vertex, names))
			return 1;
		if (_fill_vertexs(adjacency_relation, names))
			return 1;
		//if (_fill_map(name_of_vertex, names) || _fill_vertexs(adjacency_relation, names))
		//	return 1;
		_state = true;
		_names.resize(_row_size);
		for (const auto& it : names)
			_names[it.second] = it.first;
		return 0;
	}
	
	list<string> get_isolated_vertex() const
	{
		list<string> result;
		auto cbegin = _vertexs.cbegin();
		for (auto i = size_type(0); i < _row_size; ++i)
		{
			bool isLoser =
				all_of(
					cbegin + _row_size * i,
					cbegin + _row_size * (i + 1),
					[](bool b) {return !b; }
				);
			if (isLoser)
				result.push_front(_names[i]);
		}
		return result;
	}
	list<pair<const string, int>> get_degrees_vertices() const
	{
		list<pair<const string, int>> result;
		auto cbegin = _vertexs.begin();
		for (auto i = size_type(0); i < _row_size; ++i)
		{
			auto numberOfLikes = count(cbegin + i*_row_size, cbegin + (i + 1)*_row_size, true);
			result.push_back(make_pair(_names[i], numberOfLikes));
		}
		auto max = max_element(result.cbegin(), result.cend(), [](const pair<const string, int>& lhs, const pair<string, int>& rhs) {return lhs.second < rhs.second; });
		result.remove_if([&max](const pair<const string, int>& lhs) {return lhs.second != max->second; });
		return result;
	}
	list<string> get_not_symmetrical() const
	{
		list<int> vertex;
		for (auto i = size_type(0); i < _row_size; ++i)
			for (auto j = size_type(1 + i); j < _row_size; ++j)
				if (is_someone_likable(j) && _vertexs[i*_row_size + j] != _vertexs[j*_row_size + i])
					vertex.push_back((_vertexs[i*_row_size + j]) ? j : i);
		vertex.sort();
		vertex.unique();
		list<string> result;
		while (!vertex.empty()) {
			result.push_back(_names[vertex.back()]);
			vertex.pop_back();
		}
		return result;
	}
};

void usage()
{
	cout << "Please make your selection" << endl
		<< "1 - Those whom nobody loves" << endl
		<< "2 - There is no love in reply" << endl
		<< "3 - The list of popular people" << endl
		<< "4 - Quit" << endl << endl;
}
void help(const char *program) {
	cout << program;
	cout << ": Need a filename for a parameter." << endl;
	cout << "example:" << endl;
	cout << program << " filename1" << " filename2" << endl;
	/* <<
	"Options:" << endl <<
	"-h | --help        Print this help" << endl;*/
}
int main(int argc, char *argv[])
{
	if (argc < 2) { help(argv[0]); return 1; }
	usage();
	Graph graph(argv[1], argv[2]);
	if (!graph.good())
	{
		cout << "File format not correct. Check the file setup. " << endl;
		return 1;
	}
	auto choose = 0;;
	const auto width = 10;
	while (choose != 4)
	{
		cin >> choose;
		switch (choose)
		{
			case 1: {
				cout << "Those whom nobody loves:" << endl << setw(width);
				auto losers = graph.get_isolated_vertex();
				copy(losers.cbegin(), losers.cend(), ostream_iterator<string>(cout, " "));
				cout << endl;
			}
					break;
			case 2: {
				cout << "There is no love in reply:" << endl << setw(width);
				auto notLikly = graph.get_not_symmetrical();
				copy(notLikly.cbegin(), notLikly.cend(), ostream_iterator<string>(cout, " "));
				cout << endl;
			}
					break;
			case 3: {
				cout << "The list of popular people:" << endl;
				auto popular = graph.get_degrees_vertices();
				for (const auto& elem : popular)
					cout << setw(width) << "name: " << elem.first << "  \t" << " popularity: " << elem.second << endl;
			}
					break;
			default:
				break;
		}
	}
}