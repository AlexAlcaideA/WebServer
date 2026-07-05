NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP \
           -I$(HEADERSDIR) -I$(TEMPLATESDIR)

# -------------------------------------------------
# Carpetas
# -------------------------------------------------
SRCSDIR       = src
HEADERSDIR    = include
TEMPLATESDIR  = template
OBJDIR        = obj

# -------------------------------------------------
# Módulos / subdirectorios (fuentes explícitas)
# -------------------------------------------------
# Archivos sin subdirectorio
SRCS_MAIN     = main.cpp Configuration.cpp

# Archivos en src/configContext/
CONFCONTEXT_DIR  = configContext
CONFCONTEXT_SRCS = ConfigContext.cpp LocationContext.cpp

# Concatenamos todas las fuentes con sus rutas relativas a src/
SRCS = $(SRCS_MAIN) \
       $(addprefix $(CONFCONTEXT_DIR)/, $(CONFCONTEXT_SRCS))

# -------------------------------------------------
# Objetos y dependencias
# -------------------------------------------------
OBJS       = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
DEPS       = $(OBJS:.o=.d)
OBJSUBDIRS = $(sort $(dir $(OBJS)))   # obj/ obj/configContext/

# -------------------------------------------------
# Reglas
# -------------------------------------------------
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# Crear todos los subdirectorios de obj/ necesarios
$(OBJSUBDIRS):
	mkdir -p $@

# Regla de compilación: los objetos dependen de su .cpp y de que exista el subdirectorio correspondiente
$(OBJDIR)/%.o: $(SRCSDIR)/%.cpp | $(OBJSUBDIRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Incluir las dependencias generadas automáticamente por el compilador
-include $(DEPS)

# -------------------------------------------------
# Limpieza
# -------------------------------------------------
clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re