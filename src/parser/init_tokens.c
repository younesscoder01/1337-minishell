/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tokens.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysahraou <ysahraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 19:49:55 by ysahraou          #+#    #+#             */
/*   Updated: 2024/09/04 11:23:52 by ysahraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	arr_size(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return (0);
	while (arr[i])
		i++;
	return (i);
}

char	**ft_realloc(char *arg, char **old_arr)
{
	char	**new_arr;
	int		i;

	new_arr = malloc(sizeof(char *) * (arr_size(old_arr) + 2));
	i = 0;
	if (old_arr)
	{
		while (old_arr[i])
		{
			new_arr[i] = old_arr[i];
			i++;
		}
	}
	new_arr[i++] = arg;
	new_arr[i] = NULL;
	free(old_arr);
	return (new_arr);
}

void	set_size(t_token *head)
{
	int	count;
	int	i;

	while (head)
	{
		count = 0;
		while (head->args[count])
			count++;
		head->arg_size = count;
		i = 0;
		while (head->args[i])
		{
			if (is_q(head->args[i]))
				head->is_q = true;
			else
				head->is_q = false;
			i++;
		}
		head = head->next;
	}
}

void	set_type(t_token *head)
{
	while (head)
	{
		if (head->args[0][0] == '|')
			head->type = PIPE;
		else if (head->prev && (head->prev->type == RED
				|| head->prev->type == HEREDOC))
			head->type = FILE_N;
		else if (head->arg_size == 2)
		{
			if (head->args[0][0] == '<' && head->args[1][0] == '<')
				head->type = HEREDOC;
			else if (head->args[0][0] == '>' || head->args[1][0] == '>')
				head->type = RED;
			else
				head->type = CMD;
		}
		else if (head->args[0][0] == '>' || head->args[0][0] == '<')
			head->type = RED;
		else
			head->type = CMD;
		head = head->next;
	}
}

t_token	*init_tokens(char **args)
{
	int		i;
	t_token	*head;
	char	**new_args;

	head = NULL;
	i = 0;
	new_args = NULL;
	while (args && args[i])
	{
		if (ft_strchr("<>", args[i][0]))
			fun_init(&new_args, &head, args, &i);
		else if (args[i] && args[i][0] == '|')
			fun_init_v2(&new_args, &head, args, &i);
		else
		{
			new_args = ft_realloc(args[i], new_args);
			i++;
		}
	}
	if (new_args)
		add_back_t(&head, create_token(new_args));
	free(args);
	return (head);
}
