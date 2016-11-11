/*
*  Copyright 2016 Ivan Ryabov
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
/*******************************************************************************
 * @file: framework/commanalineParser.cpp
 *
 *  Created by soultaker on 18/08/16.
*******************************************************************************/

#include "solace/framework/commandlineParser.hpp"
#include "solace/framework/commandlineUtils.hpp"
#include "solace/path.hpp"

#include <fmt/format.h>

#include <cstring>
#include <cstdlib>

#include <iomanip>
#include <iostream>


using namespace Solace;
using namespace Solace::Framework;


const char CommandlineParser::DefaultPrefix = '-';



bool as_boolean(const char* v, bool* value) {
    if (strcasecmp("true", v) == 0 || strcmp("1", v) == 0) {
        *value = true;
        return true;
    }

    if (strcasecmp("false", v) == 0 || strcmp("0", v) == 0) {
        *value = false;
        return true;
    }

    return false;
}


CommandlineParser::Option::Option(char shortName, const char* longName, const char* description, int32* value):
    _shortName(shortName),
    _longName(longName),
    _description(description),
    _callback([value](Context& context) -> Optional<Error> {
        char* pEnd = nullptr;
        *value = strtol(context.value, &pEnd, 0);

        return (!pEnd || pEnd == context.value)
                ? Optional<Error>::of(fmt::format("Argument '{}' invalid int32 value: '{}'",
                                                  context.name,
                                                  context.value))  // No conversion was done!
                : None();
    }),

    _expectsArgument(OptionArgument::Required)
{
}

CommandlineParser::Option::Option(char shortName, const char* longName, const char* description, uint32* value):
    _shortName(shortName),
    _longName(longName),
    _description(description),
    _callback([value](Context& context) {
        char* pEnd = nullptr;
        *value = strtoul(context.value, &pEnd, 0);
        return (!pEnd || pEnd == context.value)
                ? Optional<Error>::of(fmt::format("Argument '{}' invalid uint32 value: '{}'",
                                                  context.name,
                                                  context.value))  // No conversion was done!
                : None();
    }),
    _expectsArgument(OptionArgument::Required)
{
}


CommandlineParser::Option::Option(char shortName, const char* longName, const char* description, float32* value) :
    _shortName(shortName),
    _longName(longName),
    _description(description),
    _callback([value](Context& context) {
        char* pEnd = nullptr;
        *value = strtof(context.value, &pEnd);
        return (!pEnd || pEnd == context.value)
                ? Optional<Error>::of(fmt::format("Argument '{}' invalid float32 value: '{}'",
                                                  context.name,
                                                  context.value))  // No conversion was done!
            : None();
    }),
    _expectsArgument(OptionArgument::Required)
{
}


CommandlineParser::Option::Option(char shortName, const char* longName, const char* description, bool* value) :
    _shortName(shortName),
    _longName(longName),
    _description(description),
    _callback([value](Context& context) {

        return (!as_boolean(context.value, value))
                ? Optional<Error>::of(fmt::format("Argument '{}' invalid boolean value: '{}'",
                                                  context.name,
                                                  context.value))  // No conversion was done!
                : None();
    }),
    _expectsArgument(OptionArgument::Optional)
{
}


CommandlineParser::Option::Option(char shortName, const char* longName, const char* description, String* value) :
    _shortName(shortName),
    _longName(longName),
    _description(description),
    _callback([value](Context& context) {
        *value = context.value;

        return None();
    }),
    _expectsArgument(OptionArgument::Required)
{
}

CommandlineParser::Option::Option(char shortName, const char* longName, const char* description,
                                  const std::function<Optional<Error> (Context& context)>& callback,
                                  OptionArgument expectsArgument) :
    _shortName(shortName),
    _longName(longName),
    _description(description),
    _callback(callback),
    _expectsArgument(expectsArgument)
{
}


bool CommandlineParser::Option::operator == (const CommandlineParser::Option& other) const noexcept {
    return ((_shortName == other._shortName)
            && (_longName == other._longName
                 || (_longName && other._longName && strcmp(_longName, other._longName) == 0)));
}



CommandlineParser::Argument::Argument(const char* name, const char* description, int32* value):
    _name(name),
    _description(description),
    _callback([value](Context& context) {
        char* pEnd = nullptr;
        *value = strtof(context.value, &pEnd);
        return (!pEnd || pEnd == context.value)
                ? Optional<Error>::of(fmt::format("Argument '{}' invalid int32 value: '{}'",
                                                  context.name,
                                                  context.value))  // No conversion was done!
            : None();
    })
{
}


CommandlineParser::Argument::Argument(const char* name, const char* description, uint32* value):
    _name(name),
    _description(description),
    _callback([value](Context& context) {
        char* pEnd = nullptr;
        *value = strtof(context.value, &pEnd);
        return (!pEnd || pEnd == context.value)
                ? Optional<Error>::of(fmt::format("Argument '{}' invalid uint32 value: '{}'",
                                                  context.name,
                                                  context.value))  // No conversion was done!
            : None();
    })
{
}


CommandlineParser::Argument::Argument(const char* name, const char* description, float32* value):
    _name(name),
    _description(description),
    _callback([value](Context& context) {
        char* pEnd = nullptr;
        *value = strtof(context.value, &pEnd);
        return (!pEnd || pEnd == context.value)
                ? Optional<Error>::of(fmt::format("Argument '{}' invalid float32 value: '{}'",
                                                  context.name,
                                                  context.value))  // No conversion was done!
            : None();
    })
{
}


CommandlineParser::Argument::Argument(const char* name, const char* description, bool* value) :
    _name(name),
    _description(description),
    _callback([value](Context& context) {
        return (as_boolean(context.value, value))
                ? Optional<Error>::of(fmt::format("Argument '{}' invalid boolean value: '{}'",
                                                  context.name,
                                                  context.value))  // No conversion was done!
            : None();
    })
{
}


CommandlineParser::Argument::Argument(const char* name, const char* description, String* value) :
    _name(name),
    _description(description),
    _callback([value](Context& context) {
        *value = context.value;

        return None();
    })
{
}


CommandlineParser::Argument::Argument(const char* name,
                                      const char* description,
                                      const std::function<Optional<Error> (Context& context)>& callback) :
    _name(name),
    _description(description),
    _callback(callback)
{
}


bool CommandlineParser::Argument::operator == (const CommandlineParser::Argument& other) const noexcept {
    return ((_description == other._description)
            || (_description && other._description && strcmp(_description, other._description) == 0));
}


CommandlineParser::CommandlineParser(const char* appDescription,
                                     const std::initializer_list<Option>& options) :
    prefix(DefaultPrefix),
    _description(appDescription),
    _options(options)
{
}


CommandlineParser::CommandlineParser(const char* appDescription,
                                     const std::initializer_list<Option>& options,
                                     const std::initializer_list<Argument>& arguments) :
    prefix(DefaultPrefix),
    _description(appDescription),
    _options(options),
    _arguments(arguments)
{
}


bool CommandlineParser::Option::isMatch(const char* name, char shortPrefix) const noexcept {
    if (_shortName && name) {
        if (name[0] == shortPrefix && name[1] == _shortName && name[2] == 0) {
            return true;
        }
    }

    if (_longName && name) {
        if (name[0] == shortPrefix && name[1] == shortPrefix && name[2] != 0 &&
                strcmp(name + 2, _longName) == 0)  {
            return true;
        }
    }

    return false;
}


Optional<Error> CommandlineParser::Option::match(Context& c) const {
    return _callback(c);
}


Optional<Error> CommandlineParser::Argument::match(Context& c) const {
    return _callback(c);
}


Result<const CommandlineParser*, Error>
fail(const std::string& message) {
    return Err<const CommandlineParser*, Error>(Error(message));
}


Result<const CommandlineParser*, Error>
CommandlineParser::parse(int argc, const char *argv[]) const {
    uint32 firstPositionalArgument = 1;

    // Handle flags
    for (int i = 1; i < argc; ++i, ++firstPositionalArgument) {
        const char* arg = argv[i];

        if (arg[0] == prefix) {
            int numberMatched = 0;
            bool valueConsumed = false;

            for (auto& option : _options) {

                if (option.isMatch(arg, prefix)) {
                    numberMatched += 1;

                    if (i + 1 < argc) {

                        const char* value = nullptr;
                        if (argv[i + 1][0] == prefix) {
                            if (option.getArgumentExpectations() == OptionArgument::Required) {
                                // Argument is required but none was given, error out!
                                return fail(fmt::format("Option '{}' expected one argument", argv[i]));
                            } else {
                                value = "true";
                            }
                        } else {
                            value = argv[i + 1];
                            valueConsumed = true;
                        }

                        Context c {argc, argv, arg, value, *this};
                        auto r = option.match(c);
                        if (r.isSome()) {
//                            return Err(std::move(r.get()));
                            return Err<const CommandlineParser*, Error>(std::move(r.get()));
                        }

                        if (c.isStopRequired) {
                            Error e("", 0);
                            return Err<const CommandlineParser*, Error>(std::move(e));
                        }

                    } else {
                        if (option.getArgumentExpectations() == OptionArgument::Required) {
                            // Argument is required but none was given, error out!
                            return fail(fmt::format("Option '{}' expected an argument and non was given.", argv[i]));
                        } else {
                            Context c {argc, argv, arg, "true", *this};
                            auto r = option.match(c);
                            if (r.isSome()) {
                                return Err<const CommandlineParser*, Error>(std::move(r.get()));
                            }

                            if (c.isStopRequired) {
                                Error e("", 0);
                                return Err<const CommandlineParser*, Error>(std::move(e));
                            }
                        }
                    }
                }
            }

            if (numberMatched < 1) {
                return fail(fmt::format("Unexpeced option '{}'", argv[i]));
            }

            if (valueConsumed) {
                ++i;
                ++firstPositionalArgument;
            }

        } else {
            // Stop processing flags
            break;
        }
    }


    // Positional arguments processing

    const uint32 nbPositionalArgument = argc - firstPositionalArgument;
    if (nbPositionalArgument > _arguments.size()) {
        return fail(fmt::format("Too many arguments given {}, expected: {}",
                                                  nbPositionalArgument,
                                                  _arguments.size()));
    }

    if (nbPositionalArgument < _arguments.size()) {
        return fail(fmt::format("No value given for argument {} '{}'",
                                                  _arguments.size() - nbPositionalArgument,
                                                  _arguments[_arguments.size() - nbPositionalArgument - 1].name()));
    }

    for (uint32 i = 0; i < nbPositionalArgument; ++i) {
        Context c {argc, argv,
                    _arguments[i].name(),
                    argv[firstPositionalArgument + i],
                    *this};

        _arguments[i].match(c);
    }


    return Ok<const CommandlineParser*, Error>(this);
}


Optional<Error> HelpFormatter::operator() (CommandlineParser::Context& c) {
    _output << "Usage: " << Path::parse(c.argv[0]).getBasename();
    if (!c.parser.options().empty()) {
        _output << " [options]";
    }

    if (!c.parser.arguments().empty()) {
        for (const auto& arg : c.parser.arguments()) {
            _output << " " << arg.name();
        }
    }
    _output << std::endl;
    _output << c.parser.description() << std::endl;

    if (!c.parser.options().empty()) {
        _output << "Options:" << std::endl;
        for (const auto& opt : c.parser.options()) {
            formatOption(c.parser.optionPrefix(), opt);
        }
    }

    c.stopParsing();
    return None();
}


void HelpFormatter::formatOption(char prefixChar, const CommandlineParser::Option& option) {
    std::stringstream s;
    s << "  ";

    if (option.shortName()) {
        s << prefixChar << option.shortName();

        if (option.name()) {
            s << ", " << prefixChar << prefixChar << option.name();
        }
    } else {
        s << prefixChar << prefixChar << option.name();
    }

    _output << std::left << std::setw(26) << s.str() << option.description() << std::endl;
}


CommandlineParser::Option CommandlineParser::printVersion(const char* appName, const Version& appVersion) {
    return {
        'v',
        "version",
        "Print version",
        VersionPrinter(appName, appVersion, std::cout),
        CommandlineParser::OptionArgument::NotRequired
    };
}

CommandlineParser::Option CommandlineParser::CommandlineParser::printHelp() {
    return {
        'h',
        "help",
        "Print help",
        HelpFormatter(std::cout),
        CommandlineParser::OptionArgument::NotRequired
    };
}
