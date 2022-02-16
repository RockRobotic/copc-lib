

### Naming Convention

#### C++

We mostly use Google's [Style Guide](https://google.github.io/styleguide/cppguide.html).
```c++

namespace name
{
class ClassName
{
public:
// Default constructor
ClassName() = default;
ClassName(int public_variable, int private_variable, bool private_read_only)
: public_variable(public_variable), private_variable_(private_variable),
private_read_only_(private_read_only){};

int public_variable{};

// Getters and Setters
void PrivateVariable(int private_variable) { private_variable_ = private_variable; }
int PrivateVariable() const { return private_variable_; }
bool PrivateReadOnly() const { return private_read_only_; }

// Any other function
int PrivateVariablePlusOne() const { return private_variable_ + 1; }
int SumPublicAndPrivate() const { return public_variable + private_variable_; }
static std::string ReturnEmptyString() { return {}; }

private:
int private_variable_{};
bool private_read_only_{false};
};
} // namespace name
```

#### Python
```python
test_class = ClassName(public_variable=1,private_variable=2,private_read_only=True)

# using pybind .def_readwrite
test_class.public_variable = 4
assert test_class.public_variable == 4

# using pybind .def_property
test_class.private_variable = 5
assert test_class.private_variable == 5

# using pybind .def_property_readonly
assert test_class.private_read_only == True

# using pybind .def
assert test_class.PrivateVariablePlusOne() == 6
assert test_class.SumPublicAndPrivate() == 9

# using pybind .def_static
assert test_class.ReturnEmptyString == ""
```
