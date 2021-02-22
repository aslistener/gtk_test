# x11 总结
- generic event 能够处理子窗口事件。 但是具体逻辑还有待验证。



# gtk 总结
## 获取窗口坐标
XGetWindowAttributes  能够获取到相对父窗口的坐标，但是无法获取窗口的屏幕坐标
XTranslateCoordinates 能够获取窗口的屏幕坐标，以及鼠标所在的子窗口，两者可以互补




