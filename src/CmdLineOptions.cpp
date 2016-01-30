//
// Created by mwo on 6/11/15.
//

#include "CmdLineOptions.h"


namespace xmreg
{
    /**
     * Take the acc and *avv[] from the main() and check and parse
     * all the options given
     */
    CmdLineOptions::CmdLineOptions(int acc, const char *avv[]) {

        positional_options_description p;

        options_description desc(
                "mymonero, recover private and public keys "
                "and address based on MyMonero mnemonic seed");

        desc.add_options()
                ("help,h", value<bool>()->default_value(false)->implicit_value(true),
                 "produce help message")
                ("mnemonic,m", value<string>(),
                 "13 word representation of the private view key from MyMonero");

        store(command_line_parser(acc, avv)
                          .options(desc)
                          .run(), vm);


        notify(vm);

        if (vm.count("help"))
        {
            if (vm["help"].as<bool>())
                cout << desc << "\n";
        }
    }

    void
    CmdLineOptions::conflicting_options(const char* opt1, const char* opt2)
    {
        if (vm.count(opt1) && !vm[opt1].defaulted()
                && vm.count(opt2) && !vm[opt2].defaulted())
            {
                throw logic_error(string("Specifly only  '")
                                  + opt1 + "' or '" + opt2 + "'. Not both.");
            }
    }


/**
     * Return the value of the argument passed to the program
     * in wrapped around boost::optional
     */
    template<typename T>
    boost::optional<T>
    CmdLineOptions::get_option(const string & opt_name) const
    {

        if (!vm.count(opt_name))
        {
            return boost::none;
        }

        return vm[opt_name].as<T>();
    }


    // explicit instantiations of get_option template function
    template  boost::optional<string>
    CmdLineOptions::get_option<string>(const string & opt_name) const;

    template  boost::optional<bool>
            CmdLineOptions::get_option<bool>(const string & opt_name) const;

}
