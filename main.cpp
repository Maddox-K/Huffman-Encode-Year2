#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>

// defining node struct
struct Node
{
	char ch;
	int freq;
	Node* left;
	Node* right;
	Node() = delete;
	Node(char c, int f) // constructor
		: ch{ c }, freq{ f }, left{ nullptr }, right{ nullptr } {}
};

// function that reads a file and returns ch frequency
std::unordered_map<char, int>
getFrequencies(const std::string& filename)
{
	std::unordered_map<char, int> um;

	// read each character and accumulate its frequency
	std::fstream file;
	file.open(filename, std::ios::in);
	char ch;
	while (file.get(ch)) {
		//std::cout << ch;
		um[ch] += 1; // accumulate frequencies of each character
	}

	file.close();

	return um;
}

// build a huffman coding tree
Node* find_min(std::vector<Node*>& vec)
{
	auto temp_node = vec.begin();
	char temp_ch = (*temp_node)->ch;
	int temp_freq = (*temp_node)->freq;

	for (auto it = vec.begin(); it != vec.end(); it++) { // for loop finds lowest frequency character in vector of nodes
		if ((*it)->freq < temp_freq) {
			temp_freq = (*it)->freq;
			temp_ch = (*it)->ch;
			temp_node = it;
		}
	}

	Node* ret = new Node(temp_ch, temp_freq); // node to be returned is created using the data of the lowest frequency node
	ret->left = (*temp_node)->left; // set left child node
	ret->right = (*temp_node)->right; // set right child node

	delete* temp_node; // delete temp_node that was created
	temp_node = vec.erase(temp_node); // erase found lowest freq node from list of nodes

	return ret; // return lowest frequency node (including all children it may have)
}

Node* build_huffman(std::unordered_map<char, int>& um)
{
	std::vector<Node*> nodes;
	for (auto& cf : um) {
		nodes.push_back(new Node(cf.first, cf.second));
	}

	while (nodes.size() > 1) { // loop until only one root node remains (the actual root of the tree)
		Node* templ = find_min(nodes); // find lowest freq node
		Node* tempr = find_min(nodes); // find second lowest freq node

		Node* temproot = new Node('#', (templ->freq + tempr->freq)); // create root node
		temproot->left = templ; // set left child node of root
		temproot->right = tempr; // set right child node of root

		nodes.push_back(temproot); // root is pushed to back of node vector
	}

	Node* root = new Node('#', (nodes.front()->freq)); // copy front node to lone node pointer
	root->left = nodes.front()->left;
	root->right = nodes.front()->right;

	delete* nodes.begin(); // delete node remaining in vector
	nodes.erase(nodes.begin()); // erase the space in the vector

	return root;
}

// funtion to return the encoding given the huffman tree as a parameter
std::unordered_map<char, std::string> encode(const Node* root, std::string temp, std::unordered_map<char, std::string>& um) {

	// tree encoding makes use of postorder traversal (I just picked one)
	if (root) {
		encode(root->left, temp += '0', um); // going left in the tree adds a zero to the encoding string
		encode(root->right, temp += '1', um); // going right adds a one to the encoding string
		if (root->ch != '#') { // check for character (leaf) nodes
			um[root->ch] = temp; // map encoding string to character in map
		}
	}
	return um;
}

// function to output the encoding to file huff.sch
void write_encoding(std::unordered_map<char, std::string> code) {

	std::ofstream ofs("huff.sch.txt");
	for (auto& en : code) {
		ofs << en.first << ": " << en.second << "\n";
	}

	ofs.close();
}

// function that takes in encoding scheme and file name and outputs encoded content to another txt file
void output_encoding(const std::unordered_map<char, std::string>& encoding, const std::string& file_name) {

	std::fstream file; // create stream for reading from file
	std::ofstream ofs("dracula.huff.txt"); // create and open stream for outputing to encoded file
	file.open(file_name, std::ios::in); // open file reading stream
	char ch; // temp char used for reading
	while (file.get(ch)) { // read from file one character at a time
		auto search = encoding.find(ch); // find corresponding ch element of map
		ofs << search->second; // output encoding string to output file
	}

	file.close(); // close both files
	ofs.close();

}

// function to decode encoded file and output contents to _decoded file
void decode(std::unordered_map<char, std::string>& encoding, const std::string encoded_file, const std::string decoded_file) {

	std::fstream ifs;
	ifs.open(encoded_file, std::ios::in); // open file an input stream
	std::ofstream ofs(decoded_file); // open file for output of decoded contents
	std::string temp;
	char temp_ch;
	while (!ifs.eof()) { // read from encoded file until end of file
		ifs.get(temp_ch); // read each character from encoded file
		temp += temp_ch; // add each character to temporary string
		for (auto& i : encoding) { // loop through entire encoding scheme
			if (i.second == temp) { // temporary string matches an encoding string
				ofs << i.first; // output the character corresponding to the match to the output file
				temp = ""; // reset temporary string back to nothing
			}
		}
	}

	ifs.close(); // close both files
	ofs.close();
}

int main()
{
	const std::string filename = "dracula.txt";
	auto charFreq = getFrequencies(filename); // get characters and their frequencies

	// test to see if all char and freq from um
	for (auto& cf : charFreq) {
		std::cout << cf.first << ": " << cf.second << "\n";
	}
	std::cout << "\n\n";

	Node* root = build_huffman(charFreq); // build huffman tree from map of characters and corresponding frequencies

	std::unordered_map<char, std::string> um;
	std::unordered_map<char, std::string> encoding = encode(root, "", um); // create encoding corresponding to tree
	for (auto& en : encoding) {
		std::cout << en.first << ": " << en.second << "\n"; // print out characters and their encoding strings for testing purposes
	}

	write_encoding(encoding);

	output_encoding(encoding, filename); // call function to output encoded characters to output file

	decode(encoding, "dracula.huff.txt", "dracula_decoded.txt");

	return 0;
}