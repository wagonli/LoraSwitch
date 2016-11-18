package com.orange.troisiemelieu.tool;

import java.io.IOException;
import java.io.UncheckedIOException;

public class Exceptions {

    public interface ThrowableSupplier<T> {
        T get() throws Exception;
    }

    public interface ThrowableRunnable {
        void run() throws Exception;
    }

    public static void uncheck(ThrowableRunnable runnable) {
        uncheck(() -> {
            runnable.run();
            return null;
        });
    }

    public static <T> T uncheck(ThrowableSupplier<T> supplier) {
        try {
            return supplier.get();
        } catch (RuntimeException e) {
            throw e;
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}
