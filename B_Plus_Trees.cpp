#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <utility>
#include <cassert>

#define endl '\n'
typedef long long ll;
using namespace std;
#define pb push_back
#define take(a,n) for(int j=0;j<n;j++) cin>>a[j];
#define give(a,n) for(int j=0;j<n;j++) cout<<a[j]<<' ';
#define all(x) x.begin(),x.end()
#define V vector<ll>

// const int M = 998244353;
const int M = 1e9+7;
const int N=2e5+10;

// find_by_order, order_of_key

ll binexp(ll a,ll b){
    ll ans=1;
    while(b){
        if(b&1){
            ans= (ans*a)%M;
        }
        a= (a*a)%M;
        b>>=1;
    }
    return ans;
}

ll m_leaf, m;

class Node {
public:
	ll cur_size = 0, size;
	bool isLeaf = 0;
	V keyValues;
	vector<Node*> pointerValues;
    Node* parent = NULL;
	Node(ll size, bool isLeaf) {
        if(isLeaf){
            keyValues.resize(size); // m_leaf keys
		    pointerValues.resize(size+1); // m_leaf pointers + block pointer
        }
		else {
            keyValues.resize(size - 1); // m-1 keys
		    pointerValues.resize(size); // m pointers
        }
		this->size = size;
		this->isLeaf = isLeaf;
	}
    ~Node(){
        this->keyValues.clear();
        this->pointerValues.clear();
    }

	void insert_key(ll key) {
		this->cur_size++;
		this->keyValues[cur_size-1] = key;
		sort(this->keyValues.begin(),this->keyValues.begin()+this->cur_size);
	}

  void insert_with_pointer(ll key, Node** left, Node** right) {

		insert_key(key);

		for(ll i = 0; i < this->cur_size; i++) {
			if(this->keyValues[i] == key) {
				this->pointerValues[i] = *left;
				for(ll j = cur_size; j >= i+2; j--) {
					this->pointerValues[j] = this->pointerValues[j-1];
				}
				this->pointerValues[i+1] = *right;
				break;
			}
		}
	}

	void delete_key(ll key) {
		for(ll i = 0; i < cur_size; i++) {
			if(keyValues[i] == key) {
				this->cur_size = this->cur_size - 1;
				for(ll j = i+1; j < size; j++) {
					this->keyValues[j-1] = this->keyValues[j];
					this->pointerValues[j] = this->pointerValues[j+1];
				}
				break;
			}
		}
	}

	ll get_pointer_from_key(ll key) {
		ll ret = cur_size;
        if(key <= keyValues[0]) return 0;
		for(ll i = 0; i < this->cur_size-1; i++) {
			if(key > this->keyValues[i] && key <= this->keyValues[i+1]) {
				ret = i+1;
				break;
			}
		}
		return ret;
	}

    ll get_pointer_from_node(Node* node) {
		for(ll i = 0; i <= size; i++) {
			if(this->pointerValues[i] == node) return i;
		}
		return -1;
	}


	void print() {
		cout << "[ ";
		for(ll i = 0; i < this->cur_size; i++) {
			cout << this->keyValues[i] << " ";
		}
		cout << "]\t\t";
	}

	
};

class BPlusTree {
public:
	Node* root;
	BPlusTree() {
		root = NULL;
	}
  // is_root(node) --> makes this value as the root
	bool is_root(Node* node) {
		return node==root;
	}

  Node* find_node(Node* node, ll key) {
      while(node && !node->isLeaf) {
          ll index = node->get_pointer_from_key(key);
          node = node->pointerValues[index];
      }
		return node;
	}

	bool searchHandler(ll key) {
		Node* node = find_node(root, key);
		if(!node) return 0;
		bool found = 0;
		for(ll i = 0; i < node->cur_size; i++) {
			found = found | (node->keyValues[i] == key);
		}
		return found;
	}

	Node* split_leaf(Node* &leaf, ll key) {
        
		V tmp = leaf->keyValues;
		tmp.pb(key);
		sort(all(tmp));

        // give(tmp,tmp.size());

		Node* newLeaf = new Node(m_leaf,1);
		ll split = (m_leaf + 2) / 2;
		ll i = 0;

		for(auto it : tmp) {
			if(i < split) leaf->keyValues[i++] = it;
			else {
                newLeaf->keyValues[i-split] = it;
                i++;
            }
		}
		leaf->cur_size = split;
		newLeaf->cur_size = (m_leaf + 1) - split;
		return newLeaf;
	}

	ll split_node(Node* &node, ll key, Node** left, Node** right) {
		V kv = node->keyValues;
        kv.pb(key);
		sort(all(kv));

		vector<Node*>pv = node->pointerValues;
		
		for(ll i = 0; i < kv.size(); i++) {
			if(kv[i] != key) continue;

			if(i != (kv.size() - 1)) {
				pv[i] = *left;
				pv.pb(*right);
				for(ll j = pv.size() - 1; j >= i+2; j--) {
					pv[j] = pv[j-1];
				}
				pv[i+1] = *right;
				break;
			}
			else {
				pv[i] = *left;
				pv.pb(*right);
			}
		}
		Node* node2 = new Node(m,false);
		ll left_size = (m - 1)/2 , newkey;
		ll right_size = m - (left_size + 1);
		ll i = 0;
		for(auto it : kv) {
			if(i < left_size) node->keyValues[i++] = it;
			else if(i == left_size) {
                newkey= it;
                i++;
            }
			else {
                node2->keyValues[i-left_size-1] = it;
                i++;
            }
		}

		node->cur_size = left_size;
		node2->cur_size = right_size;

		for(ll i = 0; i <= left_size; i++) node->pointerValues[i] = pv[i];
		for(ll i = left_size + 1; i < m; i++) node->pointerValues[i] = NULL;
		for(ll i = 0; i <= right_size; i++) node2->pointerValues[i] = pv[left_size + 1 + i];
        for(ll i = right_size + 1; i < m; i++) node2->pointerValues[i] = NULL;

        // update left and right pointers
        *left = node;
        *right = node2;
        // new key value that should be inserted in parent node
		return newkey;
	}

	void insertHandler(ll key) {
		if(root == NULL) root = new Node(m_leaf,true);
		
		ll split;
		Node* left = NULL,*right = NULL;

		if(!insert(root, key, split, left, right)) { // handle new root
			Node* newroot = new Node(m,false);
            left->parent = newroot;
            right->parent = newroot;
			newroot->insert_with_pointer(split, &left, &right);
			root = newroot;
		}
	}

    ll check_space(Node* node, ll key) {
        if(node->cur_size != node->keyValues.size()) {
            node->insert_key(key);
            return 1;
        }
        return 0;
    }

    ll insert_leaf(Node* node,ll key, ll &split, Node* &left, Node* &right){

        if(check_space(node,key)) return 1;
        
        Node* newLeaf = split_leaf(node, key);
        split = node->keyValues[node->cur_size-1];
        // cout<<split<<endl;
        newLeaf->pointerValues.back() = node->pointerValues.back();
        newLeaf->parent = node->parent;
        node->pointerValues.back() = newLeaf;
        left = node;
        right = newLeaf;

        return 0;
    }

	ll insert(Node* node, ll key, ll &split, Node* &left, Node* &right) {
		if(node->isLeaf) {
			return insert_leaf(node, key, split, left, right);
		}

		ll ind = node->get_pointer_from_key(key);

		if(insert(node->pointerValues[ind],key, split, left, right)) return 1;

		if(node->cur_size != node->keyValues.size()) {
            left->parent = node;
            right->parent = node;
			node->insert_with_pointer(split, &left, &right);
			return 1;
		}
        else {
            split = split_node(node, split, &left, &right);
            return 0;
        }
	}

	Node* get_left_sibling(Node* node,Node* parent) {
		if(parent == NULL) return NULL;
		ll ind = parent->get_pointer_from_node(node);
		if(ind>0) return parent->pointerValues[ind-1];
		else return NULL;
	}

	Node* get_right_sibling(Node* node,Node* parent) {
		if(parent == NULL) return NULL;
		ll ind = parent->get_pointer_from_node(node);
		if(ind < parent->cur_size && parent->pointerValues[ind+1]) return parent->pointerValues[ind+1];
		else return NULL;
	}

	void merge_node(Node* left,Node* right) {
		ll sz = right->cur_size,i=0;
		for(;i<sz;i++){
			left->pointerValues[left->cur_size] = right->pointerValues[i];
			left->insert_key(right->keyValues[i]);
		}
		// cout<<"left size: "<<left->cur_size<<endl;
		left->pointerValues[left->cur_size] = right->pointerValues[i];
		
		right->pointerValues[i]->parent = left;
	}
	
	ll distribute_or_merge_internal(Node* node) {
		Node* nodeParent = node->parent;

		Node* left_sibling = get_left_sibling(node,nodeParent);
		cout<<"Left sibling: \n";
		left_sibling->print();
		cout<<endl;

		Node* right_sibling = get_right_sibling(node,nodeParent);
		cout<<"Right sibling: \n";
		right_sibling->print();
		cout<<endl;

		if(left_sibling && left_sibling->cur_size > (left_sibling->size)/2 ) {
			// cout<<"here\n";
			ll lastkey = left_sibling->keyValues[left_sibling->cur_size-1]; // 6
			ll ind = nodeParent->get_pointer_from_node(left_sibling);
			ll upkey = nodeParent->keyValues[ind]; // 7
			
			// handle parent
			nodeParent->keyValues[ind] = lastkey;

			// handle pointers
			Node* p = left_sibling->pointerValues[left_sibling->cur_size];
			// p is left
			Node* right = node->pointerValues[0];
			node->insert_with_pointer(upkey,&p,&right);

			left_sibling->delete_key(lastkey);

			return 1;
		}
		else if(right_sibling && right_sibling->cur_size > (right_sibling->size)/2 ) {

			ll firstkey = right_sibling->keyValues[0];
			ll ind = nodeParent->get_pointer_from_node(right_sibling);
			ll upkey = nodeParent->keyValues[ind];

			// handle parent
			nodeParent->keyValues[ind] = firstkey;

			// handle pointers
			Node* p = right_sibling->pointerValues[1];

			// insert in node
			node->insert_key(upkey);
			node->pointerValues[node->cur_size] = p;
			
			right_sibling->delete_key(firstkey);

			return 1;
		}
		else {
			if(left_sibling){
				// cout<<"hehehe\n";
				ll ind = nodeParent->get_pointer_from_node(left_sibling);
				// cout<<ind<<"ind of left_sibling\n";
				left_sibling->insert_key(nodeParent->keyValues[ind]);
				
				merge_node(left_sibling, node);
				delete_internal(left_sibling,node);
				return 1;	
			}
			else if(right_sibling) {
				ll ind = nodeParent->get_pointer_from_node(right_sibling);
				node->insert_key(nodeParent->keyValues[ind]);
				merge_node(node,right_sibling);
				delete_internal(node,right_sibling);
				return 1;
			}
			else {
				cout<< "This is not implemented..\n";
				return 0;
			}
			
		}

	}

	ll delete_internal(Node* left, Node* node){
		Node* nodeParent = node->parent;
		

		ll ind = nodeParent->get_pointer_from_node(left);
		ll key = nodeParent->keyValues[ind];
		nodeParent->delete_key(key);
		delete node;

		if(nodeParent==root) return 1; // root is exempted
		if(nodeParent->cur_size < (nodeParent->size)/2){
			return distribute_or_merge_internal(nodeParent);
		}
		else return 1;
	}


	void merge_leaf(Node* left, Node* right){
		ll sz = right->cur_size;
		for(ll i=0;i<sz;i++) {
			left->insert_key(right->keyValues[i]);
		}
	}

	ll distribute_or_merge_leaf(Node* node, ll key) {
		
        Node* left_sibling = get_left_sibling(node,node->parent);
		cout<<"Left sibling leaf: \n";
		if(left_sibling) left_sibling->print();
		cout<<endl;
		
		Node* right_sibling = get_right_sibling(node,node->parent);
		cout<<"Right sibling leaf: \n";
		if(right_sibling)right_sibling->print();
		cout<<endl;

		if(left_sibling && left_sibling->cur_size > (left_sibling->size+1)/2 ) {

			ll lastkey = left_sibling->keyValues[left_sibling->cur_size-1]; // 12
			node->delete_key(key);
			node->insert_key(lastkey);
			left_sibling->delete_key(lastkey);
			
			Node* leftParent = left_sibling->parent; // handle parent
			ll ind = leftParent->get_pointer_from_key(lastkey);
			leftParent->keyValues[ind] = left_sibling->keyValues[left_sibling->cur_size-1];
			
			return 1;
		}
		else if(right_sibling && right_sibling->cur_size > (right_sibling->size+1)/2 ) {

			ll firstkey = right_sibling->keyValues[0];
			node->delete_key(key);
			node->insert_key(firstkey);
			right_sibling->delete_key(firstkey);

			Node* rightParent = node->parent; // handle parent
			ll ind = rightParent->get_pointer_from_key(key);
			rightParent->keyValues[ind] = firstkey;
	
			return 1;
		}
		else {
			if(left_sibling){
				node->delete_key(key);
				merge_leaf(left_sibling, node);
				return delete_internal(left_sibling,node);
			}
			else if(right_sibling) {
				node->delete_key(key);
				merge_leaf(node,right_sibling);
				return delete_internal(node,right_sibling);
			}
			else {
				cout<< "This is not implemented..\n";
				return 0;
			}
			
		}


	}

	void deleteHandler(ll key) {
		Delete(root,key);
		if(!root->cur_size) root = root->pointerValues[0];
	}

    ll delete_leaf( Node* node, ll key) {
		
        if((node->cur_size - 1) < (node->size + 1)/2) {
            if(distribute_or_merge_leaf(node,key)) return 1;
        }
        else {
			node->delete_key(key);
            return 1;
        }
        
    }

	void Delete( Node* node, ll key) {
        node->print();
		cout<<endl;
		if(node->isLeaf) {
			delete_leaf(node,key);
            return;
		}
		Delete(node->pointerValues[node->get_pointer_from_key(key)],key);
	}

	void print_directory(Node* root) {

        if(!root) return;

        queue<pair<Node*, ll>> q;
        q.push({root,0});

		ll prev = 0, i = 0;
		while(q.size()) {
			Node* node = q.front().first;
			int lvl = q.front().second;
            q.pop();

			if(lvl != prev) cout << '\n';
			prev=lvl;
			ll sz=node->cur_size;
			for(ll i=0;i<=sz;i++) {
				Node* p = node->pointerValues[i];
				if(!p) break;
				q.push({p,lvl+1});
			}
			node->print();
            i++;
		}
		cout << "\n\n";
	}

	void parentHandler(Node* root) {

        if(!root) return;

        queue<pair<Node*, ll>> q;
        q.push({root,0});

		ll prev = 0, i = 0;
		while(q.size()) {
			Node* node = q.front().first;
			int lvl = q.front().second;
            q.pop();

			if(lvl != prev) cout << '\n';
			prev=lvl;
			ll sz=node->cur_size;
			for(ll i=0;i<=sz;i++) {
				Node* p = node->pointerValues[i];
				if(!p) break;
				q.push({p,lvl+1});
				p->parent = node;
			}
            i++;
		}
		cout << "\n\n";
	}
};


void Terminal_Wizard(){
    cout<< "\n\n------Terminal Wizard------\n\n";

    cout << "CommandSyntax \tAction\n"
         << "---------------------------\n"
         << "p         \t Print the complete B+ tree structure.\n"
         << "i <value> \t Insert a key, with value 'value'.\n"
         << "d <value> \t Delete a key, with value 'value'.\n"
         << "s <value> \t Search a key, with value 'value'.\n"
         << "h         \t Print the wizard again to show available commands.\n"
         << "q         \t Quit the app.\n";
        
    cout<<"\n\nTip: Just write the command with its arguments in the prompt.\n\n";

}

signed main() {
	cout << "Enter m , m_leaf: ";
	cin >> m >> m_leaf;

	BPlusTree *bptree = new BPlusTree();
	Terminal_Wizard();
	
    char choice;

	while(1) {
        cout<<"Enter Command: > ";
		cin >> choice;
        ll key;

        switch (choice) {
            case 'i': 
                cin >> key;
                if(bptree->searchHandler(key)) {
                    cout << "\nAlready present!\n";
                }
                else {
                    bptree->insertHandler(key);
                    cout << "Successfully Inserted!\n";
					bptree->parentHandler(bptree->root);
                }
                cout << endl;
                break;
            
            case 'd':
                cin >> key;
                if(!bptree->root) {
                    cout << "\nEmpty B+ Tree!\n";
                }
                else if(bptree->searchHandler(key)) {
                    bptree->deleteHandler(key);
                    cout << "Successfully Deleted!";
					bptree->parentHandler(bptree->root);
                }
                else cout << "\nKey not present!\n";

                cout << endl;
                break;
            
            case 's':
                cin >> key;
                cout << "\nSearch Result: ";
                if(bptree->searchHandler(key)) cout << "Key Found!\n";
                else cout << "Key not found!\n";

                cout << endl;
                break;
            
            case 'p':
                cout << "\nB+ tree structure...\n";
                bptree->print_directory(bptree->root);
                break;

            case 'h':
                Terminal_Wizard();
                break;
            
            case 'q':
                cout << "\nbye!\n";
                break;
            
            default:
                cout << "\nCommand: "<<choice<<" not found! Try reading list of available commands using command 'h'\n\n";
                break;
	    }
        if(choice == 'q') break;       

    }

}