#include <iostream>
#include "trie.hpp"
#include "utils.hpp"

Trie::Trie() {
    root = std::shared_ptr<Node>(new Node());
    for (int i = 0; i < CHAR_AMOUNT; i++) {
      root->childs[i] = std::shared_ptr<Node>(nullptr);
    }
}

void Trie::add_keyword(std::string&& keyword, Token::Type type) {
  std::shared_ptr<Node> last = root;
  int size = keyword.size();

  for (int i = 0; i < size; i++) {
    if (get_child_node(last, keyword, i) == nullptr) {
      get_child_node(last, keyword, i) = std::make_shared<Node>();
    }

    last = get_child_node(last, keyword, i);
  }

  last->type = type;
}

Token::Type Trie::lookup(std::string& code, long& index) {
  std::shared_ptr<Node> actual_node = root;
  long i = index;

  while (get_child_node(actual_node, code, i) != nullptr && i < code.size() && !Utils::is_blankspace(code[i])) {
    actual_node = get_child_node(actual_node, code, i++);
  }

  if (actual_node->type != Token::Type::UNKNOWN) {
    index += i - index;
  }

  return actual_node->type;
}
