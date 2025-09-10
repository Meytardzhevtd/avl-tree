namespace my_algorithms {
template <
    typename T,
    typename Compare = std::less<T>,
    typename Allocator = std::allocator<T>>
class AvlSet {
    struct Node {
        T value;
        size_t hight;
        size_t size;
        Node *parent;
        Node *left;
        Node *right;
        Node *next;
        Node *prev;

        explicit Node(const T &value) noexcept
            : value(value),
              hight(1),
              size(1),
              parent(nullptr),
              left(nullptr),
              right(nullptr),
              next(nullptr),
              prev(nullptr) {
        }
    };

    struct iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        iterator() : node_(nullptr) {
        }

        explicit iterator(Node *node) : node_(node) {
        }

        reference operator*() const noexcept {
            return node_->value;
        }

        pointer operator->() const noexcept {
            return &(node_->value);
        }

        iterator operator++() {
            node_ = node_->next;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator operator--() {
            node_ = node_->prev;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            node_ = node_->prev;
            return tmp;
        }

        bool operator==(const iterator &other) const {
            return node_ == other.node_;
        }

        bool operator!=(const iterator &other) const {
            return node_ != other.node_;
        }

    private:
        Node *node_;
        friend struct AvlSet<T>;
    };

public:
    using key_type = T;
    using value_type = T;
    using key_compare = Compare;
    using value_compare = Compare;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer =
        typename std::allocator_traits<Allocator>::const_pointer;

    using iterator = typename AvlSet::iterator;
    using NodeAllocator =
        typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<NodeAllocator>;
    using const_iterator =
        typename AvlSet::iterator;  // у тебя итератор уже const
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() const {
        Node *v = root_;
        if (!v) {
            return iterator(nullptr);
        }
        while (v->left) {
            v = v->left;
        }
        return iterator(v);
    }

    iterator end() const {
        return iterator(nullptr);
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    iterator find(const T &value) {
        return iterator(find_(root_, value));
    }

    const_iterator find(const T &value) const {
        return const_iterator(find_(root_, value));
    }

    bool contains(const T &value) const {
        return find_(root_, value) != nullptr;
    }

    size_t count(const T &value) const {
        return contains(value) ? 1 : 0;
    }

    iterator lower_bound(const T &value) {
        Node *res = nullptr;
        Node *v = root_;
        while (v) {
            if (!comp_(v->value, value)) {
                res = v;
                v = v->left;
            } else {
                v = v->right;
            }
        }
        return iterator(res);
    }

    const_iterator lower_bound(const T &value) const {
        return const_iterator(const_cast<AvlSet *>(this)->lower_bound(value));
    }

    iterator upper_bound(const T &value) {
        Node *res = nullptr;
        Node *v = root_;
        while (v) {
            if (comp_(value, v->value)) {
                res = v;
                v = v->left;
            } else {
                v = v->right;
            }
        }
        return iterator(res);
    }

    const_iterator upper_bound(const T &value) const {
        return const_iterator(const_cast<AvlSet *>(this)->upper_bound(value));
    }

    std::pair<iterator, iterator> equal_range(const T &value) {
        return {lower_bound(value), upper_bound(value)};
    }

    std::pair<const_iterator, const_iterator> equal_range(
        const T &value
    ) const {
        return {lower_bound(value), upper_bound(value)};
    }

    void swap(AvlSet &other) noexcept {
        std::swap(root_, other.root_);
        std::swap(comp_, other.comp_);
        std::swap(allocator_, other.allocator_);
    }

    key_compare key_comp() const {
        return comp_;
    }

    value_compare value_comp() const {
        return comp_;
    }

    allocator_type get_allocator() const noexcept {
        return allocator_;
    }

    void insert(const T &value) {
        root_ = insert_(root_, value);
        update_prev_and_next(find_(root_, value));
    }

    void erase(const T &value) {
        root_ = erase_(root_, value);
    }

    void print() {
        print_(root_);
    }

    size_t size() const noexcept {
        return get_size(root_);
    }

    bool empty() const noexcept {
        return get_size(root_) == 0;
    }

    void clear() {
        destroy(root_);
        root_ = nullptr;
    }

    AvlSet() {
    }

    ~AvlSet() {
        destroy(root_);
    }

    friend bool operator==(const AvlSet &lhs, const AvlSet &rhs) {
        return lhs.size() == rhs.size() &&
               std::equal(lhs.begin(), lhs.end(), rhs.begin());
        ;
    }

    friend bool operator!=(const AvlSet &lhs, const AvlSet &rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(const AvlSet &lhs, const AvlSet &rhs) {
        return std::lexicographical_compare(
            lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), lhs.comp_
        );
    }

    friend bool operator>(const AvlSet &lhs, const AvlSet &rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(const AvlSet &lhs, const AvlSet &rhs) {
        return !(rhs < lhs);
    }

    friend bool operator>=(const AvlSet &lhs, const AvlSet &rhs) {
        return !(lhs < rhs);
    }

private:
    Node *get_next_node(Node *v) const noexcept {
        if (!v) {
            return nullptr;
        }
        if (v->right) {
            v = v->right;
            while (v->left) {
                v = v->left;
            }
            return v;
        }

        while (v->parent && v == v->parent->right) {
            v = v->parent;
        }
        return v->parent;
    }

    Node *get_prev_node(Node *v) const noexcept {
        if (!v) {
            return nullptr;
        }
        if (v->left) {
            v = v->left;
            while (v->right) {
                v = v->right;
            }
            return v;
        }
        while (v->parent && v == v->parent->left) {
            v = v->parent;
        }
        return v->parent;
    }

    void destroy(Node *v) {
        if (!v) {
            return;
        }
        destroy(v->left);
        destroy(v->right);
        NodeTraits::destroy(allocator_, v);
        NodeTraits::deallocate(allocator_, v, 1);
    }

    size_t get_hight(Node *v) const noexcept {
        return v ? v->hight : 0;
    }

    size_t get_size(Node *v) const noexcept {
        return v ? v->size : 0;
    }

    void update(Node *v) noexcept {
        v->size = 1 + get_size(v->left) + get_size(v->right);
        v->hight = 1 + std::max(get_hight(v->left), get_hight(v->right));
    }

    Node *right_rotate(Node *v) noexcept {
        Node *temp = v->left;
        v->left = temp->right;
        if (v->left) {
            v->left->parent = v;
        }
        temp->right = v;

        temp->parent = v->parent;
        v->parent = temp;

        update(v);
        update(temp);
        return temp;
    }

    Node *left_rotate(Node *v) noexcept {
        Node *temp = v->right;
        v->right = temp->left;
        if (v->right) {
            v->right->parent = v;
        }
        temp->left = v;
        temp->parent = v->parent;
        v->parent = temp;

        update(v);
        update(temp);
        return temp;
    }

    int get_balance(Node *v) const noexcept {
        if (!v) {
            return 0;
        }
        return get_hight(v->left) - get_hight(v->right);
    }

    Node *rebalance(Node *v) {
        if (!v) {
            return v;
        }
        update(v);
        int b = get_balance(v);
        if (b == 2) {
            if (get_balance(v->left) >= 0) {
                v = right_rotate(v);
            } else {
                v->left = left_rotate(v->left);
                v = right_rotate(v);
            }
        } else if (b == -2) {
            if (get_balance(v->right) <= 0) {
                v = left_rotate(v);
            } else {
                v->right = right_rotate(v->right);
                v = left_rotate(v);
            }
        }
        return v;
    }

    Node *insert_(Node *v, const T &value) {
        if (!v) {
            Node *node = NodeTraits::allocate(allocator_, 1);
            NodeTraits::construct(allocator_, node, value);
            update_prev_and_next(node);
            return node;
        }
        if (comp_(v->value, value)) {
            v->right = insert_(v->right, value);
            if (v->right) {
                v->right->parent = v;
            }
        } else if (comp_(value, v->value)) {
            v->left = insert_(v->left, value);
            if (v->left) {
                v->left->parent = v;
            }
        } else {
            return v;
        }
        return rebalance(v);
    }

    void update_prev_and_next(Node *v) {
        if (!v) {
            return;
        }
        Node *next = get_next_node(v);
        Node *prev = get_prev_node(v);
        v->next = next;
        v->prev = prev;
        if (next) {
            next->prev = v;
        }
        if (prev) {
            prev->next = v;
        }
    }

    Node *find_(Node *v, const T &value) const {
        if (!v) {
            return nullptr;
        }
        if (comp_(value, v->value)) {
            return find_(v->left, value);
        } else if (comp_(v->value, value)) {
            return find_(v->right, value);
        } else {
            return v;
        }
    }

    Node *erase_(Node *v, const T &x) {
        if (!v) {
            return nullptr;
        }

        if (comp_(x, v->value)) {
            v->left = erase_(v->left, x);
            if (v->left) {
                v->left->parent = v;
            }
        } else if (comp_(v->value, x)) {
            v->right = erase_(v->right, x);
            if (v->right) {
                v->right->parent = v;
            }
        } else {
            if (!v->left || !v->right) {
                Node *child = v->left ? v->left : v->right;

                // Обновляем prev/next
                if (v->prev) {
                    v->prev->next = v->next;
                }
                if (v->next) {
                    v->next->prev = v->prev;
                }

                if (child) {
                    child->parent = v->parent;
                }
                NodeTraits::destroy(allocator_, v);
                NodeTraits::deallocate(allocator_, v, 1);
                return child;
            } else {
                // У узла два ребёнка — ищем next (минимум в правом поддереве)
                Node *succ = v->right;
                while (succ->left) {
                    succ = succ->left;
                }

                // Обновим prev/next перед заменой
                if (succ->prev) {
                    succ->prev->next = succ->next;
                }
                if (succ->next) {
                    succ->next->prev = succ->prev;
                }

                v->value = succ->value;  // копируем значение
                v->right = erase_(v->right, succ->value);
                if (v->right) {
                    v->right->parent = v;
                }
            }
        }

        return rebalance(v);
    }

    void print_(Node *v) {
        if (!v) {
            return;
        }
        print_(v->left);
        std::cout << "-- Value = " << v->value << ", prev = " << v->prev
                  << ", this = " << v << ", next = " << v->next << '\n';
        print_(v->right);
    }

    Node *root_ = nullptr;
    Compare comp_;
    NodeAllocator allocator_;
};

}  // namespace my_algorithms