// META: script=/resources/testharness.js
// META: script=/resources/testharnessreport.js
// META: script=/gen/layout_test_data/mojo/public/js/mojo_bindings.js
// META: script=/gen/mojo/public/mojom/base/unguessable_token.mojom.js
// META: script=/gen/third_party/blink/public/mojom/serial/serial.mojom.js
// META: script=resources/serial-test-utils.js

serial_test(async (t, fake) => {
  // Wait for getPorts() to resolve in order to ensure that the Mojo client
  // interface has been configured.
  let ports = await navigator.serial.getPorts();
  assert_equals(ports.length, 0);

  [{},
   {usbVendorId: 1},
   {usbProductId: 2},
   {usbVendorId: 1, usbProductId: 2},
  ].forEach((expectedInfo) => {
    serial_test(async (t, fake) => {
      let watcher = new EventWatcher(t, navigator.serial, ['connect']);
      fake.addPort(expectedInfo);
      let evt = await watcher.wait_for(['connect']);
      let info = evt.port.getInfo();
      assert_object_equals(expectedInfo, info);
    }, `getInfo() returns ${JSON.stringify(expectedInfo)}`);
  });
}, 'getInfo() meta test');
