HTTP_TEST_DIR := ./http_test
CONF_TEST_DIR := ./conf_test

.PHONY: all
all: http conf ## Run all tests

.PHONY: http
http: ## Run http tests
	$(MAKE) -C $(HTTP_TEST_DIR)

.PHONY: conf
conf: ## Run conf tests
	bash $(CONF_TEST_DIR)/test.sh

.PHONY: install
install: ## Install packages for testing
	$(MAKE) -C $(HTTP_TEST_DIR) install

help: ## Show this message
	@echo "Target lists:"
	@grep -E '^[a-zA-Z_-]+\t*:.*?## .*$$' Makefile | awk 'BEGIN {FS = "\t*:.*?## "}; {printf "    \033[36m%-10s\033[0m %s\n", $$1, $$2}'
