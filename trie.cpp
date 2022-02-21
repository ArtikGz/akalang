#include "trie.hpp"

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
      get_child_node(last, keyword, i) = std::shared_ptr<Node>(new Node());
    }

    last = get_child_node(last, keyword, i);
  }

  last->type = type;
}

Token::Type Trie::lookup(std::string& text) {
  std::shared_ptr<Node> actual_node = root;

  int i = 0;
  while (get_child_node(actual_node, text, i) != nullptr && i < text.size()) {
    actual_node = get_child_node(actual_node, text, i++);
  }

  return actual_node->type;
}
