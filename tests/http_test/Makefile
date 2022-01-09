POETRY := $(shell which poetry)
PYTHON := python3
PROJECT_DIR := ../../

test: ## Test webserv
	$(MAKE) -C $(PROJECT_DIR)
ifndef POETRY
	source .venv/bin/activate && $(PYTHON) -m pytest && deactivate
else
	poetry run python -m pytest
endif

install: ## Install Python packages
ifndef POETRY
	$(PYTHON) -m venv .venv
	source .venv/bin/activate && pip install -r requirements.txt
else
	poetry install
endif

format: ## Format codes with black and isort
	poetry run black .
	poetry run isort .

require: ## Dump requirements.txt
	poetry export --without-hashes -o requirements.txt

help: ## Show this message
	@echo "Target lists:"
	@grep -E '^[a-zA-Z_-]+\t*:.*?## .*$$' Makefile | awk 'BEGIN {FS = "\t*:.*?## "}; {printf "    \033[36m%-10s\033[0m %s\n", $$1, $$2}'
