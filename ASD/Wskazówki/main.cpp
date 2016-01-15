#include <algorithm>
#include <cstdlib>
#include <utility>

class AvlList {
	//AvlList to implementacja dynamicznego ciągu
	//za pomocą drzewa AVL. Potrzebne są jedynie konstruktor,
	//destruktor, operacja wstawiania (O(log(n))), operacja
	//brania elementu o danym numerze (O(log(n))) oraz destruktor 
	
	//implementacja wzorowana na Cormenie
	
	private:
		class Node;
		Node *root;
	public:
		AvlList() : root(&Node::nilNode) {}
		~AvlList() { clear(); }

		int &at(size_t index) {
			return root->find(index)->val;
		}

		void insert(size_t index, const int val) {
		//wstawia wartość val na miejsce index
			root = root->insert(index, val);
		}

		void clear() {
		//czyści drzewo
			if (root != &Node::nilNode) {
				delete root;
				root = &Node::nilNode;
			}
		}

	private:
		class Node {
			public:
				static Node nilNode;
				int val;
				int height;
				//wysokość poddrzewa ukorzenionego w danym węźle
				size_t size;
				//rozmiar poddrzewa ukorzenionego w danym węźle
				Node *left;
				Node *right;
			private:
				Node() :
					val(),
					height(0),
					size(0),
					left(NULL),
					right(NULL) {}
				//konstruktor nilNode

				Node(int val) :
					val(val),
					height(1),
					size(1),
					left(&Node::nilNode),
					right(&Node::nilNode) {}				
				//konstruktor węzła na podstawie wartości

			public:
				~Node() {
					if (left != &nilNode)
						delete left;
					if (right != &nilNode)
						delete right;
				}

				Node *find(size_t index) {
					//find zwraca wartość na pozycji index w ciągu.
					//Podobnie jak w pozostałych procedurach
					//zakładamy, że argument jest poprawny i znajduje się
					//w ciągu
					size_t lSize = left->size;
					if (index < lSize)
						return left->find(index);
					else if (index > lSize)
						return right->find(index - lSize - 1);
					else
						return this;
				}

				Node *insert(size_t index, const int val) {
					if (this == &nilNode) {
						//zakładamy poprawność argumentów - nie zajmujemy się asercją
						return new Node(val);
					}
					size_t lSize = left->size;
					if (index <= lSize)
						left = left->insert(index, val);
					else
						right = right->insert(index - lSize - 1, val);
					update_info();
					return update_balance();
				}

				Node *update_balance() {
					//procedura update_balance działa dokładnie tak, jak
					//opisano w Cormenie i działa w czasie O(1), wykonując odpowiednie
					//rotacje (co najwyżej dwie)
					int balance = give_balance();
					Node *result = this;
					if (balance == 2) {
						if (right->give_balance() == -1)
							right = right->rotate_right();
						result = rotate_left();
					} else if (balance == -2) {
						if (left->give_balance() == 1)
							left = left->rotate_left();
						result = rotate_right();
					}
					return result;
				}

				Node *rotate_left() {
					Node *res = this->right;
					this->right = res->left;
					res->left = this;
					this->update_info();
					res->update_info();
					return res;
				}

				Node *rotate_right() {
					Node *res = this->left;
					this->left = res->right;
					res->right= this;
					this->update_info();
					res->update_info();
					return res;
				}

				int give_balance() const {
					return right->height - left->height;
				}
				
				void update_info() {
					//ta procedura będzie wykonywana jedynie dla wezłów z obojgiem dzieci
					//uaktualnia ona dane po zmianach w strukturze drzewa
					height = std::max(left->height, right->height) + 1;
					size = left->size + right->size + 1;
				}
		};
};

typename AvlList::Node AvlList::Node::nilNode;


class Hint {
	//wskazówka składa się ze zmiany kierunku i dystansu
	private:
		char dir;
		int dist;
	public:
		Hint() : dir(), dist() {}
		Hint(char dir, int dist) : dir(dir), dist(dist) {}
		char give_dir() const { return dir; } 
		int give_dist() const { return dist; }
};

class Position {
	//korzystamy z kierunków zgodnie z natualnym izomorfizmem
	// k -> exp(k*pi*i*1/2)
	private:
		int dir;
		int x_pos;
		int y_pos;
	public: 
		Position() : dir(0), x_pos(0), y_pos(0) {}
		Position(int dir, int x, int y) : dir(dir), x_pos(x), y_pos(y) {}
		Position(const Hint& h) {
			//potrzebny będzie konstuktor pozycji ze wskazówki
			switch (h.give_dir()) {
				case 'L':
					x_pos = (-1)*h.give_dist();
					y_pos = 0;
					dir = 3;
					break;
				case 'R':
					x_pos = h.give_dist();
					y_pos = 0;
					dir = 1;
					break;
				case 'U':
					x_pos = 0;
					y_pos = (-1)*h.give_dist();
					dir = 2;
					break;
			}
		}
		Position(const Position &) = default;
		Position(Position &&) = default;

		Position& operator=(const Position&) = default;
		Position& operator=(Position&&) = default;

		int give_dir() const { return dir; }
		int give_x() const { return x_pos; }
		int give_y() const { return y_pos; }		

		Position operator+=(const Position& p) {
			//ale również pozycję
			if (p.give_y() == 0 && p.give_x() == 0 && p.give_dir() == 0)
				return *this;
			if (x_pos == 0 && y_pos == 0 && dir == 0) {
				this->x_pos = p.give_x();
				this->y_pos = p.give_y();
				this->dir = p.give_dir();
				return *this;
			}
			switch (dir) {
				case 1:
					//prawo
					x_pos += p.give_y();
					y_pos -= p.give_x();
					break;
				case 0:
					//góra
					x_pos += p.give_x();
					y_pos += p.give_y();
					break;
				case 3:
					//lewo
					x_pos -= p.give_y();
					y_pos += p.give_x();
					break;
				case 2:
					//dół
					x_pos -= p.give_x();
					y_pos -= p.give_y();
					break;
			}
			dir = (dir + p.dir) % 4;
			return *this;
		}
		const Position operator+(const Position& arg) {
			Position result = *this;
			return result += arg;
		}
		void write() {
			//std::cout << "x_pos = " << x_pos
			//			<< " y_pos = " << y_pos
			//			<< " dir = " << dir << std::endl;
			printf("%i %i\n", x_pos, y_pos);
		}
};

int min_pow(int n) {
	//zwraca najmniejszą potęgę dwójki >= n
	int result = 1;
	while (result < n)
		result *= 2;
	return result;
}

int interval_tree_size(int n) {
	//zwraca wielkość drzewa przedziałowego
		//odpowiadającego przedziałowi [0 .. n-1]
	int result = min_pow(n);
	return  2*result - 1;
}




int main() {
	int n;
	scanf("%d", &n);
	AvlList list;	
	std::vector<std::pair<int, Hint>> hints;
	for (int i = 0; i < n; i++) {
		size_t no;
		char dir;
		int dist;
		scanf("%zu %c %d", &no, &dir, &dist);

		list.insert(no - 1, i);
		hints.push_back(std::pair<int, Hint>(0, Hint(dir, dist)));
	}
	for (int i = 0; i < n; i++) {
		hints[list.at(i)].first = i;
	}
	int tree_size = interval_tree_size(n);
	int first_leaf = (tree_size + 1)/2;
	Position interval_tree[tree_size];
	for (int i = 0; i < tree_size; i++)
		interval_tree[i] = Position();
	for (int i = 0; i < n; i++) {
		int leaf = first_leaf + hints[i].first;
		//na potrzeby rachunków każdy indeks będziemy trzymać
		//powiększony o 1 - w końcu drzewo przedziałowe zaczyna się od 1,
		//niestety w przeciwieństwie do tablicy w C++
		bool coming_from_right = ((leaf % 2) == 1);
		int curr_node = leaf / 2;
		Position answer = Position(hints[i].second);
		//std::cout << "!!!" << std::endl << std::endl;
	//	answer.write();
	//	std::cout << "!!!" << std::endl << std::endl;
		interval_tree[leaf - 1] = answer;
		while (curr_node > 0) {
			interval_tree[curr_node - 1] = interval_tree[2*curr_node - 1];
			interval_tree[curr_node - 1] += interval_tree[2*curr_node + 1 - 1];
			if (coming_from_right) {
				answer = interval_tree[2*curr_node - 1] + answer;
			}
			coming_from_right = ((curr_node % 2) == 1);
			curr_node /= 2;
		}
		answer.write();
	}
	return 0;
}
