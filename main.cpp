#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

class Domain {
public:
    explicit Domain(const std::string& domain) {
        std::string reversed = domain;
        std::reverse(reversed.begin(), reversed.end());
        reversed_domain_ = reversed;
    }

    bool operator==(const Domain& other) const {
        return reversed_domain_ == other.reversed_domain_;
    }

    bool operator<(const Domain& other) const {
        return reversed_domain_ < other.reversed_domain_;
    }

    [[nodiscard]] bool IsSubdomain(const Domain& other) const {
        return reversed_domain_.size() >= other.reversed_domain_.size() &&
               reversed_domain_.compare(0, other.reversed_domain_.size(), other.reversed_domain_) == 0 &&
               (reversed_domain_.size() == other.reversed_domain_.size() || reversed_domain_[other.reversed_domain_.size()] == '.');
    }

private:
    std::string reversed_domain_;
};

class DomainChecker {
public:
    template <typename RandomIt>
    DomainChecker(RandomIt begin, RandomIt end) {
        set<Domain> temp_domains(begin, end);
        for (const Domain& domain : temp_domains) {
            bool is_subdomain = false;
            for (const auto& forbidden : forbidden_domains_) {
                if (domain.IsSubdomain(forbidden)) {
                    is_subdomain = true;
                    break;
                }
            }
            if (!is_subdomain) {
                forbidden_domains_.insert(domain);
            }
        }
    }

    [[nodiscard]] bool IsForbidden(const Domain& domain) const {
        auto it = forbidden_domains_.lower_bound(domain);
        if (it != forbidden_domains_.end() && domain.IsSubdomain(*it)) {
            return true;
        }
        if (it != forbidden_domains_.begin()) {
            --it;
            if (domain.IsSubdomain(*it)) {
                return true;
            }
        }
        return false;
    }

private:
    set<Domain> forbidden_domains_;
};

template <typename Number>
std::vector<Domain> ReadDomains(istream& input, Number number) {
    std::vector<Domain> domains;
    for (Number i = 0; i < number; ++i) {
        std::string domain;
        getline(input, domain);
        domains.emplace_back(domain);
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);
    Number num;
    istringstream(line) >> num;
    return num;
}

void Tests() {
    //Тест на поддомен
    {
        Domain d1("gdz.ru");
        Domain d2("math.gdz.ru");
        assert(d2.IsSubdomain(d1));
        assert(!d1.IsSubdomain(d2));
    }

    //Тест на уникальность запрещённых доменов
    {
        vector<Domain> forbidden = {Domain("gdz.ru"), Domain("m.gdz.ru"), Domain("maps.me")};
        DomainChecker checker(forbidden.begin(), forbidden.end());
        assert(checker.IsForbidden(Domain("gdz.ru")));  // Запрещён
        assert(checker.IsForbidden(Domain("math.gdz.ru")));  // Поддомен
        assert(!checker.IsForbidden(Domain("freegdz.ru")));  // Не поддомен
    }

    //Тест на сложные домены
    {
        vector<Domain> forbidden = {Domain("com"), Domain("edu.ru")};
        DomainChecker checker(forbidden.begin(), forbidden.end());
        assert(checker.IsForbidden(Domain("mail.com")));  // Поддомен
        assert(!checker.IsForbidden(Domain("mail.ru")));  // Не поддомен
        assert(checker.IsForbidden(Domain("edu.ru")));  // Запрещён
        assert(checker.IsForbidden(Domain("school.edu.ru")));  // Поддомен
    }

    //Тесты на чтение
    {
        istringstream input("3\ngdz.ru\nmaps.me\ncom\n");
        vector<Domain> domains = ReadDomains(input, ReadNumberOnLine<size_t>(input));
        assert(domains.size() == 3);
        assert(domains[0] == Domain("gdz.ru"));
    }
}

int main() {
    Tests();

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
