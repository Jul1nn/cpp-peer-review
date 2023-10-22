//----------------------------CheckDateTimeValidity-----------------------------//

enum class DateError {
    YEAR_IS_TOO_SMALL,
    YEAR_IS_TOO_BIG,
    MONTH_IS_TOO_SMALL,
    MONTH_IS_TOO_BIG,
    DAY_IS_TOO_SMALL,
    DAY_IS_TOO_BIG,
    HOUR_IS_TOO_SMALL,
    HOUR_IS_TOO_BIG,
    MINUTE_IS_TOO_SMALL,
    MINUTE_IS_TOO_BIG,
    SECOND_IS_TOO_SMALL,
    SECOND_IS_TOO_BIG,
};

optional<vector<DateError>> CheckDateTimeValidity(const DateTime& dt) {
    vector<DateError> errors_list;
    errors_list.reserve(6);
    if (dt.year < 1) {
        errors_list.push_back(DateError::YEAR_IS_TOO_SMALL);
    }
    else
        if (dt.year > 9999) {
            errors_list.push_back(DateError::YEAR_IS_TOO_BIG);
        }

    if (dt.month < 1) {
        errors_list.push_back(DateError::MONTH_IS_TOO_SMALL);
    }
    else
        if (dt.month > 12) {
            errors_list.push_back(DateError::MONTH_IS_TOO_BIG);
        }

    const bool is_leap_year = (dt.year % 4 == 0) && !(dt.year % 100 == 0 && dt.year % 400 != 0);
    const array month_lengths = { 31, 28 + is_leap_year, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (dt.day < 1) {
        errors_list.push_back(DateError::DAY_IS_TOO_SMALL);
    }
    else
        if (dt.day > month_lengths[dt.month - 1]) {
            errors_list.push_back(DateError::DAY_IS_TOO_BIG);
        }

    if (dt.hour < 0) {
        errors_list.push_back(DateError::HOUR_IS_TOO_SMALL);
    }
    else
        if (dt.hour > 23) {
            errors_list.push_back(DateError::HOUR_IS_TOO_BIG);
        }

    if (dt.minute < 0) {
        errors_list.push_back(DateError::MINUTE_IS_TOO_SMALL);
    }
    else
        if (dt.minute > 59) {
            errors_list.push_back(DateError::MINUTE_IS_TOO_BIG);
        }

    if (dt.second < 0) {
        errors_list.push_back(DateError::SECOND_IS_TOO_SMALL);
    }
    else
        if (dt.second > 59) {
            errors_list.push_back(DateError::SECOND_IS_TOO_BIG);
        }

    if (!errors_list.empty()) {
        return { move(errors_list) };
    }
}

//----------------------------LoadPerson-----------------------------//

enum class DBExceptions {
    ALLOW_EXCEPTIONS,
};

auto operator""_min_age(unsigned long long value) {
    return value;
}

auto operator""_max_age(unsigned long long value) {
    return value;
}

struct DBParams {
    string_view db_name_;
    int db_connection_timeout_;
    DBLogLevel db_log_level_;
    string_view name_filter_;
    DBExceptions db_allow_exception_;
    int min_age_;
    int max_age_;

    DBParams& SetName(string_view name) {
        this->db_name_ = name;
        return *this;
    }

    DBParams& SetTimeout(int connection_timeout) {
        this->db_connection_timeout_ = connection_timeout;
        return *this;
    }

    DBParams& SetLogLevel(DBLogLevel log_level) {
        this->db_log_level_ = log_level;
        return *this;
    }

    DBParams& SetNameFilter(string_view name_filter) {
        this->name_filter_ = name_filter;
        return *this;
    }

    DBParams& SetExceptionsAllowance(DBExceptions allow_exception) {
        this->db_allow_exception_ = allow_exception;
        return *this;
    }

    DBParams& SetMinAge(int age) {
        this->min_age_ = age;
        return *this;
    }

    DBParams& SetMaxAge(int age) {
        this->max_age_ = age;
        return *this;
    }
};

vector<Person> LoadPersons(DBParams db_parameters) {
    DBConnector connector(db_parameters.db_allow_exception_, db_parameters.db_log_level_);
    DBHandler db;
    if (db_parameters.db_name_.starts_with("tmp."s)) {
        db = connector.ConnectTmp(db_parameters.db_name_, db_parameters.db_connection_timeout_);
    }
    else {
        db = connector.Connect(db_parameters.db_name_, db_parameters.db_connection_timeout_);
    }
    if (db_parameters.db_allow_exception_ && !db.IsOK()) {
        return {};
    }

    ostringstream query_str;
    query_str << "from Persons "s
        << "select Name, Age "s
        << "where Age between "s << db_parameters.min_age_ << " and "s << db_parameters.max_age_ << " "s
        << "and Name like '%"s << db.Quote(db_parameters.name_filter_) << "%'"s;
    DBQuery query(query_str.str());

    vector<Person> persons;
    for (auto [name, age] : db.LoadRows<string, int>(query)) {
        persons.push_back({ move(name), age });
    }
    return persons;
}

//----------------------------ParseCitySubjson-----------------------------//

// Дана функция ParseCitySubjson, обрабатывающая JSON-объект со списком городов конкретной страны:
void ParseCitySubjson(vector<City>& cities, const Json& json, const Country& country) {
    for (const auto& city_json : json.AsList()) {
        const auto& city_obj = city_json.AsObject();
        cities.push_back({ city_obj["name"s].AsString(), city_obj["iso_code"s].AsString(),
                          country.phone_code + city_obj["phone_code"s].AsString(), country.name, country.iso_code,
                          country.time_zone, country.languages });
    }
}

// ParseCitySubjson вызывается только из функции ParseCountryJson следующим образом:
void ParseCountryJson(vector<Country>& countries, vector<City>& cities, const Json& json) {
    for (const auto& country_json : json.AsList()) {
        const auto& country_obj = country_json.AsObject();
        countries.push_back({
            country_obj["name"s].AsString(),
            country_obj["iso_code"s].AsString(),
            country_obj["phone_code"s].AsString(),
            country_obj["time_zone"s].AsString(),
            });
        Country& country = countries.back();
        for (const auto& lang_obj : country_obj["languages"s].AsList()) {
            country.languages.push_back(FromString<Language>(lang_obj.AsString()));
        }
        ParseCitySubjson(cities, country_obj["cities"s], country);
    }
}