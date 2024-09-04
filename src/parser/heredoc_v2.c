/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_v2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysahraou <ysahraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:11:07 by ysahraou          #+#    #+#             */
/*   Updated: 2024/09/04 12:40:01 by ysahraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	set_int(int *i)
{
	i[0] = 0;
	i[1] = 0;
	i[2] = 0;
}

void	read_put(char *file_name, char *del, int q, t_list *env)
{
	char	*str;
	int		i[3];
	int		fd;

	signal_setup(1);
	fd = open(file_name, O_CREAT | O_RDWR, 0777);
	while (1)
	{
		str = readline(">");
		if (!str)
		{
			write(1, "bash: warning: here-document delimited by end-of-file!\n",
				55);
			return ;
		}
		if (ff_strncmp(str, del, ft_strlen(del)) && ft_strlen(str) != 0)
			break ;
		set_int(i);
		if (q == 0)
			str = expand(str, env, i, NULL);
		write(fd, str, ft_strlen(str));
		write(fd, "\n", 1);
		free(str);
	}
	close(fd);
}

void	fork_heredoc(char *file_name, t_token *head, t_list *env)
{
	if (is_q(head->next->args[0]))
	{
		head->next->args[0] = rm_quote(head->next->args[0]);
		read_put(file_name, head->next->args[0], 1, env);
	}
	else
		read_put(file_name, head->next->args[0], 0, env);
	free(file_name);
	exit(2);
}

void	free_re(t_token *head, char *file_name)
{
	head->args = malloc(sizeof(char *) * 2);
	head->args[0] = ft_strdup("<");
	head->args[1] = NULL;
	head->type = RED;
	head->arg_size = 1;
	free(head->next->args[0]);
	head->next->args[0] = file_name;
	head->next->type = FILE_N;
}
