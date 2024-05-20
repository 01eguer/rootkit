#pragma once
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <linux/slab.h> // For kmalloc and kfree
#include <linux/string.h>


void read_file(char *buf, char *file_name, size_t size, loff_t offset);