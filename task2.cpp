#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <iterator>
#include <cassert>

using namespace std;

class Domain {
public:
    Domain(string s)
        :name_(move(s))
    {
        reverse(name_.begin(), name_.end());
        name_.push_back('.');
    }

    friend bool operator==(const Domain& lhs, const Domain& rhs) {
        return lhs.name_ == rhs.name_;
    }

    friend bool operator<(const Domain& lhs, const Domain& rhs) {
        return lhs.name_ < rhs.name_;
    }

    const string& GetName() const {
        return name_;
    }

    // Принимает другой домен и возвращает true, если this его поддомен
    bool IsSubdomain(const Domain& main_domain) const {
        string_view sv_name(name_);
        return sv_name.substr(0, main_domain.name_.size()) == string_view(main_domain.name_);
    }

private:
    string name_;
};

class DomainChecker {
public:

    template <typename InputIt>
    DomainChecker(InputIt start, InputIt end) {
        bad_domains_.reserve(distance(start, end));
        for (auto it = start; it != end; ++it) {
            bad_domains_.push_back(*it);
        }
        sort(bad_domains_.begin(), bad_domains_.end());
        auto it_unique = unique(bad_domains_.begin(), bad_domains_.end(), [](const Domain& lhs, const Domain& rhs)
            { return rhs.IsSubdomain(lhs); });
        bad_domains_.erase(it_unique, bad_domains_.end());
    }

    // Возвращает true, если домен запрещён
    bool IsForbidden(const Domain& domain) {
        auto it = upper_bound(bad_domains_.begin(), bad_domains_.end(), domain);
        if (it == bad_domains_.begin()) {
            return false;
        }
        if (domain.IsSubdomain(*prev(it))) {
            return true;
        }
        return false;
    }

private:
    vector<Domain> bad_domains_;
};

// Читает заданное количество доменов из стандартного входа
vector<Domain> ReadDomains(istream& input, int count) {
    vector<Domain> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
        string s;
        getline(input, s);
        result.emplace_back(s);
    }
    return result;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

void Test() {
    const std::vector<Domain> forbidden_domains = { {"gdz.ru"}, {"maps.me"}, {"m.gdz.ru"}, {"com"} };
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    assert(checker.IsForbidden({ "gdz.ru" }));
    assert(checker.IsForbidden({ "gdz.com" }));
    assert(checker.IsForbidden({ "m.maps.me" }));
    assert(checker.IsForbidden({ "alg.m.gdz.ru" }));
    assert(checker.IsForbidden({ "maps.com" }));
    assert(!checker.IsForbidden({ "maps.ru" }));
    assert(!checker.IsForbidden({ "gdz.ua" }));
}

int main() {
    Test();
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}