/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omanar <omanar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/07 19:19:19 by omanar            #+#    #+#             */
/*   Updated: 2023/07/20 10:21:17 by omanar           ###   ########.fr       */
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
		std::string		_index;
		std::string		_redirect;
		std::string		_upload_path;
		bool			_directory_listing;
		bool			_methods[3];
		std::vector<std::string>		_cgi_extensions;

		Location();
};

class Config {
	public:
		int				_port;
		int				_max_body_size;
		std::string		_server_name;
		std::string		_host;
		std::map<int, std::string> _error_pages;
		std::vector<Location>	*_locations;
	
		Config();
		~Config();
		std::vector<Location>::iterator getLocation(std::string const &url);
		bool getAllowed(std::vector<Location>::iterator location);
		bool postAllowed(std::vector<Location>::iterator location);
		bool deleteAllowed(std::vector<Location>::iterator location);
};

#endif
