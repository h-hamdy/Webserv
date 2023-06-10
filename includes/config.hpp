/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/07 19:19:19 by omanar            #+#    #+#             */
/*   Updated: 2023/06/10 14:22:07 by omanar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <stdexcept>

class Location {
	public:
		std::string		_url;
		std::string		_root;
		std::string		_autoindex;
		std::string		_upload_path;
		std::string		_directory_listing;
		std::string		_cgi_pass;
		std::string		_cgi_extension;
		std::vector<std::string>	_index;
		std::vector<std::string>	_methods;

		Location();
};

class Config {
	public:
		std::string		_server_name;
		std::string		_host;
		std::string		_port;
		std::string		_max_body_size;
		std::map<int, std::string> errorPages;
		std::vector<Location>	*_locations;
	
		Config();
		~Config();
};

#endif
