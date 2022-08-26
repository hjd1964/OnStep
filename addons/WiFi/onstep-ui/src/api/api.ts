import axios, { AxiosResponse } from "axios";

export class API {
  private axios;
  constructor() {
    this.axios = axios.create({
      baseURL: `${import.meta.env.VITE_API}`,
    });
  }

  getWithoutParse(url: string, params?: object) {
    return this.axios.get<string>(url, {
      params: {
        ...(params ?? {}),
        cache: Date.now(),
      },
    });
  }

  async get<T>(url: string, params?: object) {
    return this.parseResponse<T>(await this.getWithoutParse(url, params));
  }

  async post<T>(url: string, params?: object) {
    return this.parseResponse<T>(
      await this.axios.post(url, {
        ...(params ?? {}),
      })
    );
  }

  private parseResponse<T>(response: AxiosResponse) {
    const data = response.data;

    return Object.fromEntries(
      data
        .split("\n")
        .filter(Boolean)
        .map((v: string) => {
          return v.split("|");
        })
    ) as T;
  }
}

export const api = new API();
