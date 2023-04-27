# Linux Kernel Module for FISMOS

## Generate `DTS` file

> Write-Up based on [Tutorial](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842279/Build+Device+Tree+Blob) from Xilinx

### Prepare toolchain (once)

```shell
git clone https://github.com/Xilinx/device-tree-xlnx
cd device-tree-xlnx
git checkout xilinx-v2020.2
```

### Generate files

* Source Xilinx design tools

```shell
xsct
hsi open_hw_design <design_name>.xsa
hsi set_repo_path <path to device-tree-xlnx repository>
hsi create_sw_design device-tree -os device_tree -proc psv_cortexa72_0
hsi generate_target -dir ./my_dts
hsi close_hw_design [current_hw_design]
exit
```

---

## Prepare scripts for on-board development

* `cd /usr/src/kernel` and `make modules_prepare`
* If this doesn't help: `make`
  * Wait for `HOSTCXX scripts/gcc-plugins/arm_ssp_per_task_plugin.so` to be done and cancel afterwards
  * To be sure: run `make modules_prepare` again!

---

## Get current DeviceTree Source

```shell
dtc -I fs /sys/firmware/devicetree/base > devicetree_source.dts
```

---

## Filetypes

| Extension | Filetype                                                             |
|-----------|----------------------------------------------------------------------|
|     DT    | Device Tree                                                          |
|    DTB    | Device Tree Blob                                                     |
|    DTBO   | Device Tree Blob for Overlay                                         |
|    DTC    | Device Tree Compiler                                                 |
|    DTO    | Device Tree Overlay                                                  |
|    DTS    | Device Tree Source                                                   |
|    FDT    | Flattened Device Tree, a binary format contained in a .dtb blob file |
