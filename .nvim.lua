vim.opt.errorformat = " %#%f(%l\\,%c):\\ %m"
vim.opt.makeprg = "build.bat"
vim.keymap.set("n", "<", "<cmd>tabnew term://run.bat<CR>");

vim.keymap.set("n", "<A-c>", "<cmd>make!<CR>");
vim.keymap.set("n", "<A-n>", "<cmd>cnext<CR>");
vim.keymap.set("n", "<A-p>", "<cmd>cprev<CR>");

