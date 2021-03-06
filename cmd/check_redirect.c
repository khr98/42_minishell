/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_redirect.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hokim <hokim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/09 16:56:56 by hyerkim           #+#    #+#             */
/*   Updated: 2021/05/23 15:38:53 by hokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char		*get_filename_from(char *str)
{
	char	*result;
	int		i;
	int		j;

	result = malloc(ft_strlen(str) + 1);
	i = -1;
	j = -1;
	while (str[++i] == ' ')
		;
	while (str[i] && str[i] != ' ' && str[i] != '>' && str[i] != '<')
	{
		result[++j] = str[i++];
		str[i - 1] = ' ';
	}
	result[++j] = '\0';
	return (result);
}

int			redirect_err(int fd, char *filename, int status)
{
	ft_write(fd, ERR_HEADER);
	ft_write(fd, filename);
	ft_write(fd, ": ");
	ft_write(fd, strerror(errno));
	ft_write(fd, "\n");
	return (status);
}

int			write_redirect(char *pipe, t_datas *datas, t_fd *fd, int i)
{
	char	**str;
	int		create;
	char	*filename;

	str = NULL;
	pipe[i] = ' ';
	create = 0;
	if (pipe[i + 1] && pipe[i + 1] == '>')
	{
		i++;
		pipe[i] = ' ';
		create = O_WRONLY | O_APPEND | O_CREAT;
	}
	else
		create = O_WRONLY | O_TRUNC | O_CREAT;
	filename = get_filename_from(pipe + i + 1);
	fd->write = open(filename, create, S_IRWXU | S_IRWXG | S_IRWXO);
	if (datas->fd.write != -1)
		return (1);
	datas->fd.write = 1;
	str = ft_split(pipe, ' ');
	datas->status = redirect_err(datas->ori_fd.err, filename, 1);
	free_str_array(str);
	free(filename);
	return (0);
}

int			read_redirect(char *pipe, t_datas *datas, t_fd *fd, int i)
{
	char	**str;
	char	*filename;

	pipe[i] = ' ';
	str = ft_split(pipe, ' ');
	filename = get_filename_from(pipe + i + 1);
	fd->read = open(filename, O_RDONLY);
	free(filename);
	if (datas->fd.read == -1)
	{
		datas->status = redirect_err(datas->ori_fd.err, filename, 1);
		free_str_array(str);
		datas->fd.read = 0;
		return (0);
	}
	if (str != NULL)
		free_str_array(str);
	return (1);
}

int			check_redirect(char *pipe, t_datas *datas)
{
	int		i;
	int		result;

	i = -1;
	while (pipe[++i])
	{
		if (pipe[i] == '>')
		{
			if (!(result = write_redirect(pipe, datas, &datas->fd, i)))
				break ;
		}
		else if (pipe[i] == '<')
		{
			if (!(result = read_redirect(pipe, datas, &datas->fd, i)))
				break ;
		}
	}
	return (result);
}
