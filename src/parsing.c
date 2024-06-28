/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysahraou <ysahraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 18:30:37 by ysahraou          #+#    #+#             */
/*   Updated: 2024/06/28 18:48:53 by ysahraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
// int check_cmd(char *cmd)
// {
    
// }

char	**get_paths(char **env)
{
	char	*paths;
	int		i;
    char	*first_part;
	char	**the_paths;

	i = 0;
	paths = NULL;
	while (env[i])
	{
		paths = ft_strnstr(env[i], "PATH", 4);
		if (paths)
			break ;
		i++;
	}
	if (paths == NULL)
		return (NULL);

	i = 0;
	while (paths[i] && paths[i] != '=')
		i++;
	first_part = ft_substr(&paths[i + 1], 0, ft_strlen(paths));
	the_paths = ft_split(first_part, ':');
	free(first_part);
	return (the_paths);
}
